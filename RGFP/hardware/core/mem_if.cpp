#include "mem_if.h"

SC_HAS_PROCESS(CombinedMemoryInterface);
/**
 * @brief Constructor
 */
CombinedMemoryInterface::CombinedMemoryInterface
(sc_core::sc_module_name name
, sc_core::sc_time end_rsp_delay
, unsigned int max_txns)
	:sc_module(name)
	,m_send_end_rsp_PEQ("m_send_end_rsp_PEq")
	,m_end_rsp_delay(end_rsp_delay)
	,m_active_txn_count(0)
{
	isock.register_nb_transport_bw(this, &CombinedMemoryInterface::nb_transport_bw);

	SC_THREAD(end_response_thread);

	for (unsigned int i = 0; i < max_txns; i++ )
	{
		m_transaction_queue.enqueue ();
	}
}


/**
 * @brief Begin Request Thread
 */
void CombinedMemoryInterface::_do_transaction(tlm::tlm_command cmd, uint64_t addr, uint8_t *data, unsigned num_bytes)
//{{{
{
	tlm::tlm_generic_payload * trans_ptr;
	unsigned char			 * data_ptr;

	while(m_transaction_queue.is_empty()); //transaction pool中要有可用的transaction
	trans_ptr = m_transaction_queue.dequeue();
	trans_ptr->acquire();
	++m_active_txn_count;
	data_ptr = trans_ptr->get_data_ptr();
	*reinterpret_cast<unsigned int*>(data_ptr) = *(unsigned int *)data;
//	*reinterpret_cast<unsigned int*>(data_ptr) = *data; //TODO: 深入研究
//	有的时候正常，有的时候只写入低位；

	trans_ptr->set_command(cmd);
	trans_ptr->set_address(addr);
//	trans_ptr->set_data_ptr(data);
	if(cmd == tlm::TLM_READ_COMMAND)
	{
		trans_ptr->set_data_ptr(data);
	}else
	{
		trans_ptr->set_data_ptr(data_ptr);	
	}
	trans_ptr->set_data_length(num_bytes);

	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
	tlm::tlm_sync_enum return_value = isock->nb_transport_fw(*trans_ptr, phase, delay);

	switch (return_value)
	{
		case tlm::TLM_ACCEPTED:
			wait(mEndRequestEvent);
			break;
		default:
			{
				std::cout<< "return_value:" << return_value << std::endl;
				assert(false && "mem_if.h: unsupported protocol, only 4 phase transaction supported");	
			}
	}
	wait(mEndResponseEvent); 
}
	//}}}
	
/**
 * @brief Response Thread
 */
void CombinedMemoryInterface::end_response_thread()
{
	while(true)
	{
		wait(m_send_end_rsp_PEQ.get_event());
		tlm::tlm_generic_payload *trans_ptr;
		if(	(trans_ptr = m_send_end_rsp_PEQ.get_next_transaction()) != NULL)	
		{
			tlm::tlm_phase phase = tlm::END_RESP;
			sc_core::sc_time delay = sc_core::SC_ZERO_TIME; 	
			tlm::tlm_sync_enum return_value;
			return_value = isock->nb_transport_fw(*trans_ptr, phase, delay);

			switch(return_value)
			{
				case tlm::TLM_COMPLETED:
					mEndResponseEvent.notify(sc_core::SC_ZERO_TIME);
					break;	
				case tlm::TLM_ACCEPTED:
				case tlm::TLM_UPDATED:
				default:
					assert(false && "Unknow return value for END_RESP");
					break;
			}
			trans_ptr->release();
			--m_active_txn_count;
		}
	}
}

tlm::tlm_sync_enum CombinedMemoryInterface::nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay)
{
	if(phase == tlm::END_REQ)
	{
		mEndRequestEvent.notify(sc_core::SC_ZERO_TIME);	
	}else if( phase == tlm::BEGIN_RESP)
	{
		delay += m_end_rsp_delay;
		m_send_end_rsp_PEQ.notify(trans, delay);	
	}	
	else
	{
		assert(false && "mem_if.cpp :unrecoginized phase");
	}
	return tlm::TLM_ACCEPTED;
}
