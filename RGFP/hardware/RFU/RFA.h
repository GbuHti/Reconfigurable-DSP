#ifndef RFA_H
#define RFA_H

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

class RFA : public sc_core::sc_module
{
	private:
		

	public:
		tlm_utils::simple_target_socket<RFA> tsock;

		RFA
			(
			 sc_module_name name
			);

		tlm::tlm_sync_enum nb_transport_fw
			(
			 tlm::tlm_generic_payload &trans,
			 tlm::tlm_phase &phase,
			 sc_core::sc_time &delay		
			);
};

#endif
