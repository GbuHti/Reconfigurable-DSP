#include "memory.h"

Memory::Memory
(sc_core::sc_module_name name
, uint32_t size
, const sc_core::sc_time read_response_delay
, const sc_core::sc_time write_response_delay
)
:sc_core::sc_module(name)
, m_data(new uint8_t[size])
, m_size(size)
, m_read_delay(read_response_delay)
, m_write_delay(write_response_delay)
{
	memset(m_data, 0, size_t(m_size));	
}

void Memory::operation(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay)
//{{{
{
	tlm::tlm_command cmd		= trans.get_command();
	uint64_t addr				= trans.get_address();
	unsigned char * data_ptr	= trans.get_data_ptr();
	unsigned int len			= trans.get_data_length();	

	if(cmd == tlm::TLM_READ_COMMAND)
	{
		memcpy(data_ptr, m_data+addr, len);
		delay += m_read_delay;
	}else if(cmd == tlm::TLM_WRITE_COMMAND)
	{
		memcpy(m_data+addr, data_ptr, len);	
//		std::cout << "=> ADDR: " << addr << " DATA: " << *(float *)data_ptr << " @ " << sc_time_stamp()<< endl;
		delay += m_write_delay;
	}else
	{
		std::cout << "TLM_READ_COMMAND: " << tlm::TLM_READ_COMMAND << std::endl;
		std::cout << "TLM_WRITE_COMMAND: " << tlm::TLM_WRITE_COMMAND << std::endl;
		std::cout << "cmd              : " << cmd << std::endl;
		assert(false && "Unsupported Memory operation!");	
	}
}
//}}}

sc_core::sc_time Memory::get_device_delay(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay)
//{{{
{
	tlm::tlm_command cmd = trans.get_command();
	
	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			return delay += m_read_delay;
		case tlm::TLM_WRITE_COMMAND:
			return delay += m_write_delay;
		default:
			assert(false && "Unsupported Memory operation!");	
	}
	return delay;	
}
//}}}
