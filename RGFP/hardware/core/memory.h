#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include "stdint.h"

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"

struct Memory:public sc_core::sc_module
{
	tlm_utils::simple_target_socket<Memory> tsock;
	
	SC_HAS_PROCESS(Memory);
	
	Memory(sc_core::sc_module_name name, uint32_t size)
	:sc_core::sc_module(name)
	, m_data(new uint8_t[size])
	, m_size(size)
	{
		tsock.register_b_transport(this, &Memory::b_transport);	
	}	

	uint8_t *m_data;
	uint32_t m_size;

void write_data(unsigned addr, uint8_t *src, unsigned num_bytes) {
    assert (addr + num_bytes <= m_size);

    memcpy(m_data + addr, src, num_bytes);
}

void read_data(unsigned addr, uint8_t *dst, unsigned num_bytes) {
    assert (addr + num_bytes <= m_size);

    memcpy(dst, m_data + addr, num_bytes);
}

void b_transport(tlm::tlm_generic_payload &gp, sc_core::sc_time &delay)
{
	uint64_t addr = gp.get_address();
	unsigned char * data_ptr = gp.get_data_ptr();
	unsigned int	len = gp.get_data_length();
	tlm::tlm_command cmd = gp.get_command();

	if(cmd == tlm::TLM_READ_COMMAND){
		read_data(addr, data_ptr, len);	
	}else if ( cmd == tlm::TLM_WRITE_COMMAND){
		write_data(addr, data_ptr, len);
	}else {
		assert(false && "Memory: unsupport command");
	}
	
	delay += sc_core::sc_time(10, sc_core::SC_NS);
}
};


#endif
