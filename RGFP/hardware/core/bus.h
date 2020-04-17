#ifndef BUS_H
#define BUS_H

#include <iostream>
#include <map>

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_get.h"

struct PortMapping
{
	uint64_t m_start;
	uint64_t m_end;

	PortMapping(uint64_t start, uint64_t end)	
	: m_start(start),m_end(end)
	{
		assert(end > start);	
	}

	bool contains(uint64_t addr)
	{
		return addr >= m_start && addr < m_end;
	}

	uint64_t global_to_local(uint64_t addr)
	{
		return addr - m_start;	
	}
};

/**
 * @brief 总线
 * 将接收交易的地址与外设地址区间进行匹配，
 * 发送到相应的端口
 */
template <int NR_OF_INITIATORS,int NR_OF_TARGETS>
struct Bus: public sc_core::sc_module
{
	tlm_utils::simple_target_socket_tagged<Bus> tsock[NR_OF_INITIATORS];
	tlm_utils::simple_initiator_socket_tagged<Bus> isock[NR_OF_TARGETS];
	PortMapping * ports[NR_OF_TARGETS];
	
	tlm_utils::peq_with_get<tlm::tlm_generic_payload> mRequestPEQ;	
	tlm_utils::peq_with_get<tlm::tlm_generic_payload> mResponsePEQ;	
	sc_core::sc_event mEndRequestEvent;
	sc_core::sc_event mEndResponseEvent;

	struct ConnectionInfo {
		tlm_utils::simple_target_socket_tagged<Bus>* from;
		tlm_utils::simple_initiator_socket_tagged<Bus> * to;
	};
	typedef std::map<tlm::tlm_generic_payload *, ConnectionInfo> PendingTransactions;
	typedef typename PendingTransactions::iterator PendingTransactionsIterator;
	typedef typename PendingTransactions::const_iterator PendingTransactionsConstIterator;
	PendingTransactions mPendingTransactions;

	SC_HAS_PROCESS(Bus);
	Bus(sc_core::sc_module_name name)
		:sc_core::sc_module(name)
		 ,mRequestPEQ("mRequestPEQ")
		 ,mResponsePEQ("mResponsePEQ")
		 //{{{
	{
		for(unsigned int i=0; i<NR_OF_INITIATORS; i++)
		{
			tsock[i].register_nb_transport_fw(this, &Bus::nb_transport_fw,i);	
		}		
		for(unsigned int i=0; i<NR_OF_TARGETS; i++)
		{
			isock[i].register_nb_transport_bw(this, &Bus::nb_transport_bw,i);	
		}		

		SC_THREAD(RequestThread);
		SC_THREAD(ResponseThread);
	}
	//}}}

//	void RequestThread();
void RequestThread()
///{{{
{
	while(true)
	{
		wait(mRequestPEQ.get_event());
		tlm::tlm_generic_payload *trans;
		while((trans = mRequestPEQ.get_next_transaction())!= 0)
		{
			unsigned isock_id = decode(trans->get_address());	
			assert(isock_id < NR_OF_TARGETS);
			trans->set_address(ports[isock_id]->global_to_local(trans->get_address()));
			
			PendingTransactionsIterator it = mPendingTransactions.find(trans);
			assert(it != mPendingTransactions.end());
			it->second.to = &isock[isock_id];

			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
			tlm::tlm_sync_enum return_value = isock[isock_id]->nb_transport_fw(*trans,phase,delay);

			switch(return_value)
			{
				case tlm::TLM_COMPLETED:
					mResponsePEQ.notify(*trans, delay);
					break;
				case tlm::TLM_ACCEPTED:
				case tlm::TLM_UPDATED:
					if(phase == tlm::BEGIN_REQ)
					{
						wait(mEndRequestEvent);	
					}else if(phase == tlm::END_REQ)
					{
						wait(delay);	
					}else if( phase == tlm::BEGIN_RESP)
					{
						mResponsePEQ.notify(*trans, delay);	
					}else 
					{
						assert(false && "Unmatched phase and TLM_UPDATED");	
					}

					if(it->second.from)
					{
						phase = tlm::END_REQ;
						delay = sc_core::SC_ZERO_TIME;
						return_value = (*it->second.from)->nb_transport_bw(*trans, phase, delay);
					}
					break;
				default:
					assert(false && "Bus: Illegl return value received from target ");
					break;
			}
		}	
	}	
}
//}}}


//	void ResponseThread();
void ResponseThread()
//{{{
{
	while(true)
	{
		wait(mResponsePEQ.get_event());
		tlm::tlm_generic_payload *trans_ptr;  
		while( (trans_ptr = mResponsePEQ.get_next_transaction()) != 0)	
		{
			PendingTransactionsIterator it = mPendingTransactions.find(trans_ptr);
			assert(it != mPendingTransactions.end());
			
			uint64_t addr = trans_ptr->get_address();
			tlm::tlm_sync_enum return_value;
			tlm::tlm_phase phase = tlm::BEGIN_RESP;
			sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
			return_value = (*it->second.from)->nb_transport_bw(*trans_ptr,phase,delay);		
			
			switch(return_value)
			{
				case tlm::TLM_COMPLETED:
					wait(delay);
					break;
				case tlm::TLM_UPDATED:
				case tlm::TLM_ACCEPTED:
					wait(mEndResponseEvent);
					break;		
			}
			if(it->second.to != 0)
			{
				phase = tlm::END_RESP;
				delay = sc_core::SC_ZERO_TIME;	
				return_value = (*it->second.to)->nb_transport_fw(*trans_ptr, phase, delay);
				assert( return_value == tlm::TLM_COMPLETED);
			}

			mPendingTransactions.erase(it);
		}
	}
}
//}}}
	
//	tlm::tlm_sync_enum nb_transport_fw(int id, tlm::tlm_generic_payload &gp, tlm::tlm_phase &phase, sc_core::sc_time &delay);
tlm::tlm_sync_enum nb_transport_fw(int id, tlm::tlm_generic_payload &gp, tlm::tlm_phase &phase, sc_core::sc_time &delay)
//{{{
{
	if(phase == tlm::BEGIN_REQ)	{
		addPendingTransaction(gp, isock, id);
		mRequestPEQ.notify(gp, delay);	
	} else if( phase == tlm::END_RESP)
	{
		mEndResponseEvent.notify(sc_core::SC_ZERO_TIME);	
		return tlm::TLM_COMPLETED;
	}
	else
	{
		std::cout <<"ERROR: '" << __FUNCTION__ << "': Illegal phase received from mem_if" << std::endl;	
		assert(false);
	}

	return tlm::TLM_ACCEPTED;
	
}
//}}}


//	tlm::tlm_sync_enum nb_transport_bw(int id, tlm::tlm_generic_payload &gp, tlm::tlm_phase &phase, sc_core::sc_time &delay);
tlm::tlm_sync_enum nb_transport_bw(int id, tlm::tlm_generic_payload &gp, tlm::tlm_phase &phase, sc_core::sc_time &delay)
//{{{
{

	uint64_t addr = gp.get_address(); //for debug
	if(phase == tlm::END_REQ)
	{
		mEndRequestEvent.notify(sc_core::SC_ZERO_TIME);	
	}else if( phase == tlm::BEGIN_RESP)
	{
		mResponsePEQ.notify(gp, delay);	
	}else
	{
		assert(false && "ERROR: Illegal phase received from target.");	
	}
	return tlm::TLM_ACCEPTED;
}
//}}}
	
//	unsigned decode(uint64_t addr);
unsigned decode(uint64_t addr)
//{{{
{
	for(unsigned i = 0; i < NR_OF_TARGETS; i++)
	{
		if(ports[i]->contains(addr))
		{
			return i;	
		}	
	}	
	assert(false && "unable to find target port!!");
}
//}}}


	void addPendingTransaction(tlm::tlm_generic_payload &trans,
								tlm_utils::simple_initiator_socket_tagged<Bus> *to,
								int initiatorID)
	{
		ConnectionInfo info = {&tsock[initiatorID], to};
		assert(mPendingTransactions.find(&trans) == mPendingTransactions.end());
		mPendingTransactions[&trans] = info;	
	}
};

#endif
