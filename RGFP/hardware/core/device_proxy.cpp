#include "device_proxy.h"

DeviceProxy::DeviceProxy
(sc_core::sc_module_name name
, const sc_core::sc_time accept_delay
, Device * device
)
:sc_core::sc_module(name)
, m_accept_delay(accept_delay)
, mEndRequestPEQ("mEndRequestEvent")
, mResponsePEQ("mResponsePEQ")
, m_device(device) 
{
	tsock.register_nb_transport_fw(this, &DeviceProxy::nb_transport_fw);	
	
	SC_THREAD(end_request_thread);
	SC_THREAD(begin_response_thread);

}

tlm::tlm_sync_enum DeviceProxy::nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay)
//{{{
{
	tlm::tlm_sync_enum return_value = tlm::TLM_COMPLETED;
	switch(phase)
	{
		case tlm::BEGIN_REQ: 
			{
				sc_core::sc_time PEQ_delay_time = delay + m_accept_delay;
				mEndRequestPEQ.notify(trans, PEQ_delay_time);	
				return_value = tlm::TLM_ACCEPTED;
				break;
			}
		case tlm::END_RESP:
			{
				m_end_resp_rcvd_event.notify(delay);
				return_value = tlm::TLM_COMPLETED;
				break;
			}
		default:
			{
				assert(false && "Illegal phase in forward path");
			}
	}
	return return_value;
}
//}}}

void DeviceProxy::end_request_thread()
//{{{
{
	while(true)
	{
		wait(mEndRequestPEQ.get_event());
		tlm::tlm_generic_payload * trans_ptr;
		while((trans_ptr = mEndRequestPEQ.get_next_transaction()) != 0)
		{
			uint64_t addr = trans_ptr->get_address(); //for debug
			sc_core::sc_time delay = sc_core::SC_ZERO_TIME;	
			m_device->get_device_delay(*trans_ptr, delay);
			mResponsePEQ.notify(*trans_ptr, delay);

			tlm::tlm_sync_enum return_value;
			tlm::tlm_phase phase = tlm::END_REQ;
			delay = sc_core::SC_ZERO_TIME;
			
			return_value = tsock->nb_transport_bw(*trans_ptr, phase, delay);
			switch(return_value)
			{
				case tlm::TLM_ACCEPTED:
				{
					break;	
				}	
				case tlm::TLM_COMPLETED:
				case tlm::TLM_UPDATED:
				default:
				{
					assert(false );	
				}	
			}
		}	
	}
}
//}}}

void DeviceProxy::begin_response_thread()
//{{{
{
	while(true){
		wait(mResponsePEQ.get_event());
		tlm::tlm_generic_payload * trans_ptr;
		while((trans_ptr = mResponsePEQ.get_next_transaction()) != 0)
		{
			uint64_t addr = trans_ptr->get_address(); //for debug
			sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
			tlm::tlm_sync_enum return_value;
			tlm::tlm_phase phase = tlm::BEGIN_RESP;

			m_device->operation(*trans_ptr, delay);

			delay = sc_core::SC_ZERO_TIME;
			return_value = tsock->nb_transport_bw(*trans_ptr, phase, delay);	

			switch(return_value)
			{
				case tlm::TLM_ACCEPTED:
					{
						wait(m_end_resp_rcvd_event);	
						break;
					}	
				case tlm::TLM_COMPLETED:
					{
						wait(delay);	
						break;
					}
				default:
					{
						assert(false);	
						break;
					}
			}
		}
	}

}
//}}}
