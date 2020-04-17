#ifndef DEVICE_H
#define DEVICE_H

#include "systemc.h"
#include "tlm.h"

struct Device
{
	virtual void operation(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay)=0;	
	virtual sc_core::sc_time get_device_delay(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay)=0;
};

#endif
