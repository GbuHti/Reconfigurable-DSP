#include "clint_if.h"

void Clint::operation(tlm::tlm_generic_payload &gp, sc_core::sc_time &delay)
{
	tlm::tlm_command	cmd			= gp.get_command();
	uint64_t			addr		= gp.get_address();
	uint32_t *			data_ptr	= (uint32_t *)gp.get_data_ptr();
	
	assert( gp.get_data_length() == 4);

	delay += clock_cycle;
	sc_core::sc_time now = sc_core::sc_time_stamp();

	std::map<uint64_t, uint32_t *>::iterator it = addr_to_reg.find(addr);
	if( it != addr_to_reg.end())
	{	
		if( cmd == tlm::TLM_READ_COMMAND)
		{
			mtime = now.value()/scaler;
			*data_ptr = *(it->second);		
		}	
		else if (cmd == tlm::TLM_WRITE_COMMAND)
		{
			assert(addr != 0xbff8 && addr != 0xbffc);
			*(it->second) = *data_ptr;		
			irq_event.notify(delay);
		}
		else 
		{
			assert(false && "Unsupported tlm command for Clint access");		
		}
	}
	else
	{
		assert(false && "Wrong access address");	
	}
}

sc_core::sc_time Clint::get_device_delay(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay)
{
	return delay += m_clint_delay;
}

uint64_t Clint::update_and_get_mtime() 
{
	auto now = sc_core::sc_time_stamp().value()/scaler;
	if(now > mtime)
		mtime = now;
	
	return mtime;
}

void Clint::run()
{
	while(true)
	{
		sc_core::wait(irq_event);
		if(mtimecmp > 0 && mtime >= mtimecmp)
		{
			m_target->trigger_timer_interrupt(true);		
		}
		else
		{
			m_target->trigger_timer_interrupt(false);
			if(mtimecmp > 0)
			{
				auto time = sc_core::sc_time::from_value(mtime * scaler);
				auto goal = sc_core::sc_time::from_value(mtimecmp * scaler);
				irq_event.notify(goal - time);
			}
		}
	}	

}
