#ifndef BUS_H
#define BUS_H

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

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
template <int NR_OF_INITIATORS, int NR_OF_TARGETS>
struct Bus: public sc_core::sc_module
{
	tlm_utils::simple_target_socket_tagged<Bus> tsock[NR_OF_INITIATORS];
	tlm_utils::simple_initiator_socket_tagged<Bus> isock[NR_OF_TARGETS];
	PortMapping * ports[NR_OF_TARGETS];

	SC_HAS_PROCESS(Bus);
	Bus(sc_core::sc_module_name name)
	:sc_core::sc_module(name)
	{
		for(unsigned int i=0; i<NR_OF_INITIATORS; i++)
		{
			tsock[i].register_b_transport(this, &Bus::b_transport,i);	
		}		

	}

	unsigned decode(uint64_t addr)
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

	void b_transport( tlm::tlm_generic_payload &gp, sc_core::sc_time &delay)
	{
		uint64_t addr = gp.get_address();	
		unsigned id = decode(addr);
			
		gp.set_address(ports[id]->global_to_local(addr));
		isock[id].b_transport(gp,delay);
		
	}
};

#endif
