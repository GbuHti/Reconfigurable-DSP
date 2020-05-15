#include "arith_PE.h"
#include <cmath>

SC_HAS_PROCESS(Arith_PE);
Arith_PE::Arith_PE
( sc_module_name name
, unsigned id 
, bool binocular
)
: sc_module (name)
, m_ID(id)
, m_binocular(binocular)
, m_ini_done(false)
, m_pool_size(6)
, m_advance_computing(0)
, m_ready(false)
, mSendPEQ("mSendPEQ")
, mSrc_a_PEQ("mSrc_a_PEQ")
, mSrc_b_PEQ("mSrc_b_PEQ")
, slcs(0)
{
	for(unsigned i = 0; i < 2; i++)
	{
		tsock[i].register_nb_transport_fw(this, &Arith_PE::nb_transport_fw, i);	
	}	
	isock.register_nb_transport_bw(this, &Arith_PE::nb_transport_bw);

	for (unsigned int i = 0; i < m_pool_size+1; i++ )
	{
		m_transaction_queue.enqueue ();
	}

	SC_THREAD(wait_data_thread);
	SC_THREAD(execute_thread);
	SC_THREAD(send_thread);
	SC_THREAD(monitor_advance_comp_thread);
}

/*=================================================*/
void Arith_PE::write_context_reg(slc context)
{
	if(context.phid == m_ID)
	{
		m_ready		= true;
		m_op		= context.op;
		m_op_aux	= context.op_aux;	
	}
}

/*=================================================*/
void Arith_PE::all_config()
{
	if(m_ready)
	{
		m_ready		= false;	
		m_ini_done =  true;
	}
}

/*=================================================*/
tlm::tlm_sync_enum 
Arith_PE::nb_transport_fw
( int id
, tlm::tlm_generic_payload &trans
, tlm::tlm_phase &phase
, sc_time &delay
)
{
	assert( phase == tlm::BEGIN_REQ);
	if(id == 0)
	{
		mSrc_a_PEQ.notify(trans, delay);	
	}else
	{
		mSrc_b_PEQ.notify(trans, delay);	
	}
	return tlm::TLM_ACCEPTED;
}

/*=========================================================*/
/**
 * @brief: 
 * 判断是否是最后一次运算
 */
void Arith_PE::wait_data_thread()
{
	while(true)
	{
		if(m_binocular)	wait( mSrc_a_PEQ.get_event() & mSrc_b_PEQ.get_event() );
		else wait( mSrc_a_PEQ.get_event() ); 
		mCheckAdvanceComEvent.notify(SC_ZERO_TIME);
		wait( mProceedEvent); ///<  实质是要求m_advance_comput的值小于那个
		m_advance_computing++;

		tlm::tlm_generic_payload * trans_ptr_a;
		tlm::tlm_generic_payload * trans_ptr_b;	

		//get transaction for PEQ and ensure that there is only one transaction in PEQ
		trans_ptr_a = mSrc_a_PEQ.get_next_transaction();
		assert(mSrc_a_PEQ.get_next_transaction() == 0);

		trans_ptr_b = mSrc_b_PEQ.get_next_transaction();
		assert(mSrc_b_PEQ.get_next_transaction() == 0);

		if((trans_ptr_a->get_command() == LAST_COMPUTE) | (trans_ptr_b->get_command() == LAST_COMPUTE))
		{
			m_ini_done = false;
		}
		
		m_srca = *(float *)trans_ptr_a->get_data_ptr();
		cout << "@" << sc_time_stamp() << " m_srca: " << m_srca << endl;
		m_srcb = *(float *)trans_ptr_b->get_data_ptr();
		cout << "@" << sc_time_stamp() << " m_srcb: " << m_srcb << endl;

		mExecuteEvent.notify(SC_ZERO_TIME);

		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		sc_time	delay = SC_ZERO_TIME;
		assert( tsock[0]->nb_transport_bw(*trans_ptr_a, phase, delay) == tlm::TLM_ACCEPTED); //反馈
		assert( tsock[1]->nb_transport_bw(*trans_ptr_b, phase, delay) == tlm::TLM_ACCEPTED); //反馈
	}
}

//=======================================================
void Arith_PE::execute_thread()
{
	while(true)
	{
		wait(mExecuteEvent);
		float tmp = 0;
		sc_time compute_latency;
		switch(m_op)
		{
			case CONFIG_ADD:
				if(m_op_aux == 0){
					tmp = m_srca + m_srcb;
					compute_latency = 6*clock_period;
				}
				else{
					tmp = m_srca - m_srcb;
					compute_latency = 6*clock_period;
				}
				break;
			case CONFIG_MUL:
				if(m_op_aux == 0){
					tmp = m_srca * m_srcb;
					compute_latency = 3*clock_period;
				}
				else{
					tmp = m_srca / m_srcb;
					compute_latency = 14*clock_period;
				}
				break;
			case CONFIG_SQRT:
				tmp = sqrt(m_srca);
				compute_latency = 12*clock_period;
				break;
			case CONFIG_SIN:
				if(m_op_aux == 0){
					tmp = sin(m_srca);
					compute_latency = 36*clock_period;
				}
				else{
					tmp = cos(m_srca);
					compute_latency = 36*clock_period;
				}
				break;
			default:
				assert(false && "opcode not supported!!");
		}
		tlm::tlm_generic_payload	* trans_ptr;
		unsigned char				* data_ptr;
		assert( !m_transaction_queue.is_empty());
		trans_ptr	= m_transaction_queue.dequeue();
		trans_ptr->acquire();	
		data_ptr	= trans_ptr->get_data_ptr();
		*reinterpret_cast<float *>(data_ptr) = tmp; //TODO：check if get the right value during debug;

		tlm::tlm_command cmd;
		if(!m_ini_done)
			cmd = LAST_COMPUTE;
		else 
			cmd = NORMAL_COMPUTE;
		trans_ptr->set_command(cmd);
		
		mSendPEQ.notify(*trans_ptr, compute_latency);
	}
}

//========================================================
void Arith_PE::send_thread()
{
	while(true)
	{
		wait(mSendPEQ.get_event());
		tlm::tlm_generic_payload * trans_ptr;
		while( (trans_ptr = mSendPEQ.get_next_transaction()) != 0) ///< 循环保证不错过重叠的事件
		{
			m_advance_computing--; //此时刻计算结果已经被发送出去 实质是自减
			mCheckAdvanceComEvent.notify(SC_ZERO_TIME);
			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			sc_time delay = SC_ZERO_TIME;
			tlm::tlm_sync_enum return_value;
			return_value = isock->nb_transport_fw(*trans_ptr, phase, delay);
			assert(return_value == tlm::TLM_ACCEPTED);	

			wait(mBeginResponseEvent);
			trans_ptr->release();
			if( (!m_ini_done) & (m_advance_computing == 0)){ //当没有后续的数据到来可以认为计算任务全部结束
				assert(slcs != 0 && "You shoud connect PE with RC");
				slcs->release_busy(m_ID);	
			}

		}	
	}
}

//========================================================
tlm::tlm_sync_enum 
Arith_PE::nb_transport_bw
( tlm::tlm_generic_payload &trans
 , tlm::tlm_phase &phase
 , sc_time &delay
)
{
	assert(phase == tlm::BEGIN_RESP);
	mBeginResponseEvent.notify(SC_ZERO_TIME);

	return tlm::TLM_ACCEPTED;
}

//========================================================
void Arith_PE::monitor_advance_comp_thread()
{
	while(true)
	{
		wait(mCheckAdvanceComEvent);
		if(m_advance_computing < m_pool_size)
		{
			mProceedEvent.notify(SC_ZERO_TIME);
		}
	}
}

