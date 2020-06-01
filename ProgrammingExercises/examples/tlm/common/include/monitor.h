#ifndef __MONITOR_H__
#define __MONITOR_H__

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"


class monitor : public sc_core::sc_module
{
	public:
		monitor
		( sc_core::sc_module_name name
	 	);

		void 
		monitor_thread
		( void
		);

	private:
		typedef	tlm::tlm_generic_payload * gp_ptr;
		tlm::tlm_response_status gp_status;

	public:
		tlm_utils::simple_initiator_socket<monitor> monitor_isocket ;
};
#endif
