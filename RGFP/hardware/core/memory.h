#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include "stdint.h"

#include "systemc.h"
#include "tlm.h"

#include "device.h"

struct Memory:public sc_core::sc_module
			  ,public Device
{
	uint8_t *m_data;
	uint32_t m_size;
	sc_core::sc_time m_read_delay;
	sc_core::sc_time m_write_delay;

	SC_HAS_PROCESS(Memory);

	Memory(sc_core::sc_module_name name
			, uint32_t size
			, const sc_core::sc_time read_response_delay
			, const sc_core::sc_time write_response_delay
			);

	void operation(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) override;

	sc_core::sc_time get_device_delay(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) override;
};


#endif
