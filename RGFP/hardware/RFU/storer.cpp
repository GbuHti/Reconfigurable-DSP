#include "storer.h"
#include <iomanip>

SC_HAS_PROCESS(Storer);
Storer::Storer
( sc_module_name name
, unsigned id
)
: sc_module(name)
, m_ID(id)
, m_ini_done(false)
, m_pool_size(2)
, m_addr_counter(0)
, m_advance_computing(0)
, mResultPEQ("mResultPEQ")
, mBufferPEQ("mBufferPEQ")
{
	tsock.register_nb_transport_fw(this, &Storer::nb_transport_fw);
	isock.register_nb_transport_bw(this, &Storer::nb_transport_bw);

	for (unsigned int i = 0; i < m_pool_size+1; i++ )
	{
		m_transaction_queue.enqueue ();
	}


	SC_THREAD(wait_data_thread);
	SC_THREAD(fill_buffer_thread);
	SC_THREAD(send_data_thread);
	SC_THREAD(monitor_pool_thread);
}

/*=================================================*/
void Storer::write_context_reg(slc context)
{
	if(context.phid == m_ID)
	{
		m_op		= context.op;
		m_op_aux	= context.op_aux;	
		m_addr_inc	= context.addr_inc;
		m_addr		= context.addr;
	}
}

/*=================================================*/
void Storer::all_config()
{
	m_ini_done = true;
}

/*=================================================*/
tlm::tlm_sync_enum 
Storer::nb_transport_fw
( tlm::tlm_generic_payload &trans
, tlm::tlm_phase &phase
, sc_time &delay
)
{
	assert( phase == tlm::BEGIN_REQ);

	mResultPEQ.notify(trans, delay);	

	return tlm::TLM_ACCEPTED;
}

/*=================================================*/
void Storer::wait_data_thread()
{
	while(true)
	{
		wait( mResultPEQ.get_event() ); 
		mCheckAdvanceComEvent.notify(SC_ZERO_TIME);
		wait( mProceedEvent); ///<  实质是要求m_advance_comput的值小于那个
		m_advance_computing++;

		tlm::tlm_generic_payload * trans_ptr;

		//get transaction for PEQ and ensure that there is only one transaction in PEQ
		trans_ptr = mResultPEQ.get_next_transaction();
		assert(mResultPEQ.get_next_transaction() == 0);

		if(trans_ptr->get_command() == LAST_COMPUTE)
		{
			m_ini_done = false;
		}
		
		m_result = *(float *)trans_ptr->get_data_ptr();
		cout << "@" << sc_time_stamp() << " m_result: " << m_result << endl;

		mSendDataEvent.notify(SC_ZERO_TIME);

		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		sc_time	delay = SC_ZERO_TIME;
		assert( tsock[0]->nb_transport_bw(*trans_ptr, phase, delay) == tlm::TLM_ACCEPTED); //反馈
	}
}

//========================================================
void Storer::fill_buffer_thread()
{
	while(true)
	{
		wait(mSendDataEvent);
		tlm::tlm_generic_payload	* trans_ptr;
		unsigned char				* data_ptr;

		assert( !m_transaction_queue.is_empty());
		trans_ptr	= m_transaction_queue.dequeue();
		trans_ptr->acquire();
		data_ptr	= trans_ptr->get_data_ptr();
		*reinterpret_cast<float *>(data_ptr) = m_result; 

		tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
		sc_dt::uint64		addr = m_addr*4096 + (m_addr_counter++)*m_addr_inc*4;	
		trans_ptr->set_command(cmd);
		trans_ptr->set_address(addr);
		trans_ptr->set_data_length(4);

		mBufferPEQ.notify(*trans_ptr, SC_ZERO_TIME);
	}
}

//=========================================================
void Storer::send_data_thread()
{
	while(true)
	{
		wait(mBufferPEQ.get_event());
		tlm::tlm_generic_payload * trans_ptr;	
		while( (trans_ptr = mBufferPEQ.get_next_transaction()) != 0 )
		{
			m_advance_computing--;
			mCheckAdvanceComEvent.notify(SC_ZERO_TIME);
			
			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			sc_time delay = SC_ZERO_TIME;
			tlm::tlm_sync_enum return_value;
			return_value = isock->nb_transport_fw(*trans_ptr, phase, delay);
			assert(return_value == tlm::TLM_ACCEPTED);		

			wait(mBeginResponseEvent);
			phase	= tlm::END_RESP;
			delay	= SC_ZERO_TIME;
			return_value = isock->nb_transport_fw(*trans_ptr, phase, delay);
			assert(return_value == tlm::TLM_COMPLETED);
			cout << "### " << m_ID;
			cout << setw(200) << "Addr " << setfill('-')<< hex << trans_ptr->get_address();
			cout << setw(5) << " Data " << *(float*)trans_ptr->get_data_ptr() ;
			cout << "@ " << setw(5) << sc_time_stamp() << endl;

			trans_ptr->release();
			if( (!m_ini_done) & (m_advance_computing == 0))
			{
				assert(slcs != 0 && "You shoud connect Storer with RC");
				m_addr_counter = 0;
				slcs->release_busy(m_ID);	
			}			
		}
	}
}

//========================================================
tlm::tlm_sync_enum 
Storer::nb_transport_bw
( tlm::tlm_generic_payload &trans
 , tlm::tlm_phase &phase
 , sc_time &delay
)
{
	if(phase == tlm::END_REQ)
	{
		return tlm::TLM_ACCEPTED;
	}
	else if( phase == tlm::BEGIN_RESP)
	{
		mBeginResponseEvent.notify(SC_ZERO_TIME);
	}else
	{
		assert(false);	
	}

	return tlm::TLM_ACCEPTED;
}

//========================================================
void Storer::monitor_pool_thread()
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


