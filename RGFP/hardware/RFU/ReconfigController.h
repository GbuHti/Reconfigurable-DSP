#ifndef RECONFIGCONTROLLER_H
#define RECONFIGCONTROLLER_H

#include "systemc.h"
#include "contextreg_if.h"
#include "slcs_if.h"
#include "device.h"
#include "tlm_utils/simple_initiator_socket.h"

#include <map>

class ReconfigController: public sc_core::sc_module
					    , public Device
						, public slcs_if

{
	private:
		flc m_flc;
		std::map<unsigned, slc> m_slcs;
		std::map<unsigned, unsigned> m_translation_table;

		unsigned m_raw_slcs_num;
		uint32_t m_dp; //dispatch pending

		sc_event begin_monitor_busy_event;
		sc_event end_monitor_busy_event;	
		sc_event busy_changed_event;

		sc_event begin_allocate_event;
		sc_event finish_allocate_event;

		sc_event dispatch_event;

		tlm::tlm_generic_payload m_trans;
		uint32_t * m_data_ptr;
		
	public:
		contextreg_if * m_rfa_ptr;

		ReconfigController
		( sc_module_name name
		, contextreg_if * rfa_ptr
		);

		void operation(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) override;

		sc_core::sc_time get_device_delay(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) override;

		void Allocate_thread();

		void Dispatch_thread();

		void Monitor_busy_thread();

		void release_busy(unsigned id) override;
};

#endif
