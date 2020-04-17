#include "bus.h"
#include <iostream>

//template<int NR_OF_INITIATORS, int NR_OF_TARGETS>
//unsigned Bus<NR_OF_INITIATORS,NR_OF_TARGETS>::decode(uint64_t addr)
////{{{
//{
//	for(unsigned i = 0; i < NR_OF_TARGETS; i++)
//	{
//		if(ports[i]->contains(addr))
//		{
//			return i;	
//		}	
//	}	
//	assert(false && "unable to find target port!!");
//}
////}}}
//
//template<int NR_OF_INITIATORS, int NR_OF_TARGETS>
//tlm::tlm_sync_enum Bus<NR_OF_INITIATORS,NR_OF_TARGETS>::nb_transport_fw(int id, tlm::tlm_generic_payload &gp, tlm::tlm_phase &phase, sc_core::sc_time &delay)
////{{{
//{
//	if(phase == tlm::BEGIN_REQ)	{
//		addPendingTransaction(&gp, isock, id);
//		mRequestPEQ.notify(gp, delay);	
//	} else if( phase == tlm::END_RESP)
//	{
//		mEndResponseEvent.notify(sc_core::SC_ZERO_TIME);	
//		return tlm::TLM_COMPLETED;
//	}
//	else
//	{
//		std::cout <<"ERROR: '" << __FUNCTION__ << "': Illegal phase received from mem_if" << std::end;	
//		assert(false);
//	}
//
//	return tlm::TLM_COMPLETED;
//	
//}
////}}}
//
//template<int NR_OF_INITIATORS, int NR_OF_TARGETS>
//tlm::tlm_sync_enum Bus<NR_OF_INITIATORS, NR_OF_TARGETS>::nb_transport_bw(int id, tlm::tlm_generic_payload &gp, tlm::tlm_phase &phase, sc_core::sc_time &delay)
////{{{
//{
//	if(phase == tlm::END_REQ)
//	{
//		mEndRequestEvent.notify(sc_core::SC_ZERO_TIME);	
//	}else if( phase == tlm::BEGIN_RESP)
//	{
//		mResponsePEQ.notify(gp, delay);	
//	}else
//	{
//		assert(false && "ERROR: Illegal phase received from target.");	
//	}
//	return tlm::TLM_ACCEPTED;
//}
////}}}
//		
//
//template<int NR_OF_INITIATORS, int NR_OF_TARGETS>
//void Bus<NR_OF_INITIATORS,NR_OF_TARGETS>::RequestThread()
/////{{{
//{
//	while(true)
//	{
//		wait(mRequestPEQ.get_event());
//		tlm::tlm_generic_payload *trans;
//		while((trans = mRequestPEQ.get_next_transaction())!= 0)
//		{
//			unsigned isock_id = decode(trans->get_address());	
//			assert(isock_id < NR_OF_TARGETS);
//			trans->set_address(ports[isock_id]->global_to_local(trans->get_address()));
//			
//			PendingTransactionsIterator it = mPendingTransactions.find(trans);
//			assert(it != mPendingTransactions.end());
//			it->second.to = &isock[isock_id];
//
//			tlm::tlm_phase phase = tlm::BEGIN_REQ;
//			sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
//			tlm::tlm_sync_enum return_value = isock[isock_id]->nb_transport_fw(*trans,phase,delay);
//
//			switch(return_value)
//			{
//				case tlm::TLM_COMPLETED:
//					mResponsePEQ.notify(*trans, delay);
//					break;
//				case tlm::TLM_ACCEPTED:
//				case tlm::TLM_UPDATED:
//					if(phase == tlm::BEGIN_REQ)
//					{
//						wait(mEndRequestEvent);	
//					}else if(phase == tlm::END_REQ)
//					{
//						wait(delay);	
//					}else if( phase == tlm::BEGIN_RESP)
//					{
//						mResponsePEQ.notify(*trans, delay);	
//					}else 
//					{
//						assert(false && "Unmatched phase and TLM_UPDATED");	
//					}
//
//					if(it->second.from)
//					{
//						phase = tlm::END_REQ;
//						delay = sc_core::SC_ZERO_TIME;
//						return_value = (*it->second.from)->nb_transport_bw(*trans, phase, delay);
//					}
//					break;
//				default:
//					assert(false && "Bus: Illegl return value received from target ");
//					break;
//			}
//		}	
//	}	
//}
////}}}
//
//template<int NR_OF_INITIATORS, int NR_OF_TARGETS>
//void Bus<NR_OF_INITIATORS, NR_OF_TARGETS>::ResponseThread()
////{{{
//{
//	while(true)
//	{
//		wait(mResponsePEQ.get_event());
//		tlm::tlm_generic_payload *trans_ptr;  
//		while( (trans_ptr = mResponsePEQ.get_next_transaction()) != 0)	
//		{
//			PendingTransactionsIterator it = mPendingTransactions.find(trans_ptr);
//			assert(it != mPendingTransactions.end());
//			
//			tlm::tlm_sync_enum return_value;
//			tlm::tlm_phase phase = tlm::BEGIN_RESP;
//			sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
//			return_value = (*it->second.from)->nb_transport_bw(*trans_ptr,phase,delay);		
//			
//			switch(return_value)
//			{
//				case tlm::TLM_COMPLETED:
//					wait(delay);
//					break;
//				case tlm::TLM_UPDATED:
//				case tlm::TLM_ACCEPTED:
//					wait(mEndResponseEvent);
//					break;		
//			}
//			if(it->second.to != 0)
//			{
//				phase = tlm::END_RESP;
//				delay = sc_core::SC_ZERO_TIME;	
//				return_value = (*it->second.to)->nb_transport_fw(*trans_ptr, phase, delay);
//				assert( return_value == tlm::TLM_COMPLETED);
//			}
//
//			mPendingTransactions.erase(it);
//		}
//	}
//}
////}}}
