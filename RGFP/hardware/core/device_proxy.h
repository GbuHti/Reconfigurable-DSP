#ifndef DEVICE_PROXY_H
#define DEVICE_PROXY_H

#include <iostream>
#include "stdint.h"

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_get.h"

#include "device.h"

struct DeviceProxy:public sc_core::sc_module
{
	tlm_utils::simple_target_socket<DeviceProxy> tsock;
	
	sc_core::sc_time m_accept_delay;
	tlm_utils::peq_with_get<tlm::tlm_generic_payload> mEndRequestPEQ;
	tlm_utils::peq_with_get<tlm::tlm_generic_payload> mResponsePEQ;
	sc_core::sc_event m_end_resp_rcvd_event;

	Device * m_device;
	
	SC_HAS_PROCESS(DeviceProxy);

	DeviceProxy(sc_core::sc_module_name name
			, const sc_core::sc_time accept_delay
			, Device * device
			);

	tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &gp, tlm::tlm_phase &phase, sc_core::sc_time &delay);

	void end_request_thread();

	void begin_response_thread();	

};


#endif
