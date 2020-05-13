#include "distributor.h"

SC_HAS_PROCESS(Distributor);
Distributor::Distributor
( sc_module_name name
, unsigned id
)
: sc_module (name)
, m_ID (id)
, m_ini_done(false)
, mBeginRequestPEQ("mBeginRequestPEQ")
, mBeginResponsePEQ("mBeginResponsePEQ")
{
	tsock.register_nb_transport_fw(this, &Distributor::nb_transport_fw);
	for(int i = 0; i < 2*ARITH_PE_NUM+LOADER_NUM; i++)
	{
		isock[i].register_nb_transport_bw(this, &Distributor::nb_transport_bw,i);	
	}
	for(int i = 0; i < (int)m_branch_tb.size(); i++)
	{
		m_branch_tb[i] = {false,false,false};
	}

	SC_THREAD(request_thread);
	SC_THREAD(response_thread);
	SC_THREAD(self_starting_thread);

	m_data = 0;
}

/**
 * 每个isock端口在m_branch_tb中都有一个对应的flag
 * flag有效时，表示分发到该flag对应的端口
 * flag位由配置字配置
 */
void Distributor::write_context_reg(slc context)
{
//	assert(context.mux_a == m_ID || context.mux_b == m_ID); //context在广播模式下，此语句无用
	
	unsigned portid = 0;
	if(isArith(context))
	{
		if(context.mux_a == m_ID)
		{
			portid = 2*(context.phid - LOADER_NUM) + 0;
			m_branch_tb[portid] = {true,false,false}; 
			if(context.mux_a == context.phid) //如果arith_pe接收来自自己的结果数据,在初始化需要“推”一把
			{
				mSelfStartingEvent.notify(SC_ZERO_TIME);
			}
		}
		else if(context.mux_b == m_ID)
		{
			portid = 2*(context.phid - LOADER_NUM) + 1;
			m_branch_tb[portid] = {true,false,false}; 
			if(context.mux_b == context.phid) 
			{
				mSelfStartingEvent.notify(SC_ZERO_TIME);
			}
		}
	}else if(isStore(context))
	{
		if(context.mux_a == m_ID)
		{
			portid = context.phid - LOADER_NUM + ARITH_PE_NUM;	
			m_branch_tb[portid] = {true, false, false};
		}	
	}
	
}

void Distributor::all_config()
{
	m_ini_done = true;	
}

void Distributor::request_thread()
{
	while(true)
	{
		wait(mBeginRequestPEQ.get_event());
		tlm::tlm_generic_payload *trans_ptr;
		while((trans_ptr = mBeginRequestPEQ.get_next_transaction()) != 0 )
		{
			if(trans_ptr->get_command() == LAST_COMPUTE) m_ini_done = false;

			for(int i = 0; i < (int)m_branch_tb.size(); i++)
			{
				tlm::tlm_phase phase;
				sc_time delay;
				tlm::tlm_sync_enum return_value;
				if(m_branch_tb[i].selected)
				{
					m_branch_tb[i].hasSend = true;
					phase = tlm::BEGIN_REQ;
					delay = SC_ZERO_TIME;
					return_value = isock[i]->nb_transport_fw(*trans_ptr, phase, delay);

					switch(return_value)
					{
						case tlm::TLM_ACCEPTED:
							break;
						case tlm::TLM_UPDATED:
						case tlm::TLM_COMPLETED:
						default:
							assert(false && "not supported");	
					}
				}	
			}
		}
	}
}

void Distributor::response_thread()
{
	while(true)
	{
		wait(mBeginResponsePEQ.get_event());
		tlm::tlm_generic_payload * trans_ptr;
		int hasSend_cnt ;
		int hasReceive_cnt;
		while( (trans_ptr = mBeginResponsePEQ.get_next_transaction()) != 0)
		{
			hasSend_cnt = 0;
			hasReceive_cnt = 0;
			for(int i = 0; i < (int)m_branch_tb.size(); i++)
			{
				flags tmp = m_branch_tb[i];
				if(tmp.selected & tmp.hasSend) hasSend_cnt +=1;
				if(tmp.selected & tmp.hasReceive) hasReceive_cnt +=1;
			}	

			if(hasSend_cnt == hasReceive_cnt && hasSend_cnt != 0)
			{
				for(int i = 0; i < (int)m_branch_tb.size(); i++)
				{
					if(!m_ini_done) m_branch_tb[i].selected = false;
					m_branch_tb[i].hasSend = false;
					m_branch_tb[i].hasReceive = false;
				}	

				tlm::tlm_phase phase = tlm::BEGIN_RESP;
				sc_time delay		 = SC_ZERO_TIME;
				tlm::tlm_sync_enum return_value;
				return_value = tsock->nb_transport_bw(*trans_ptr, phase, delay);
				assert(return_value == tlm::TLM_ACCEPTED);
			}
		}
	}
}

tlm::tlm_sync_enum 
Distributor::nb_transport_fw
( tlm::tlm_generic_payload &trans
, tlm::tlm_phase &phase
, sc_time &delay
)
{
	if( phase == tlm::BEGIN_REQ)
	{
		mBeginRequestPEQ.notify(trans,delay);
	}else if( phase == tlm::END_RESP)
	{
		assert(false && "not supported");	
	}
	return tlm::TLM_ACCEPTED;
}

tlm::tlm_sync_enum 
Distributor::nb_transport_bw
( int id
, tlm::tlm_generic_payload &trans
, tlm::tlm_phase &phase
, sc_time &delay
)
{
	m_branch_tb[id].hasReceive = true;
	if( phase == tlm::BEGIN_RESP )
	{
		mBeginResponsePEQ.notify(trans, delay);	
	}else if( phase == tlm::END_REQ)
	{
		assert(false && "not support");	
	}
	return tlm::TLM_ACCEPTED;
}

//========================================================================
void Distributor::self_starting_thread()
{
	while(true)
	{
		wait(mSelfStartingEvent);
		m_self_starting_trans.set_data_ptr((unsigned char *) &m_data);
		m_self_starting_trans.set_command(NORMAL_COMPUTE);
		m_self_starting_trans.set_data_length(4);
		mBeginRequestPEQ.notify(m_self_starting_trans, SC_ZERO_TIME); //如果成功的话这种注入的技术得研究一下
	}	
}
