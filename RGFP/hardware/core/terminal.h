#ifndef TERMINAL_H
#define TERMINAL_H

#include "systemc"

#include "tlm_utils/simple_target_socket.h"

#include "device.h"

struct Terminal : public sc_core::sc_module 
				  , public Device
{
	sc_core::sc_time m_terminal_delay;

	Terminal(
			sc_core::sc_module_name name
			,sc_core::sc_time terminal_delay
			) 
		:sc_core::sc_module(name)
		,m_terminal_delay(terminal_delay)
	{
		
	}

	void operation(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) override
	{
		sc_assert (trans.get_command() == tlm::TLM_WRITE_COMMAND);
		sc_assert (trans.get_data_length() == 1);

		std::cout << "Terminnal:" << (char)*trans.get_data_ptr() << std::endl;
	}

	sc_core::sc_time get_device_delay(tlm::tlm_generic_payload & trans, sc_core::sc_time &delay) override
	{
		return delay+=m_terminal_delay;
	}	
};


#endif 
