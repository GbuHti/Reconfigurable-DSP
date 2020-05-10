#ifndef STORER_H
#define STORER_H

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_get.h"

#include "contextreg_if.h"
#include "define.h"
#include "slcs_if.h"

using namespace std;
using namespace sc_core;

class Storer : public sc_module
			 , public contextreg_if
{
	private:
		unsigned m_ID;
		unsigned m_op;
		unsigned m_op_aux;
		unsigned m_addr_inc;
		unsigned m_addr;
		bool m_binocular;

		bool m_ini_done;
		unsigned m_pool_size;

	public:
		slcs_if * slcs; ///< the interface between Loader and Reconfiguration Controller

		tlm_utils::simple_target_socket<Storer> tsock;
		tlm_utils::simple_initiator_socket<Storer> isock;

		Storer
		( sc_module_name name
		, unsigned id
	    , bool binocular	
		);	

		tlm::tlm_sync_enum 
		nb_transport_fw
		( int id
		, tlm::tlm_generic_payload &trans
		, tlm::tlm_phase &phase
		, sc_time &delay
		);

		tlm::tlm_sync_enum 
		nb_transport_bw
		( tlm::tlm_generic_payload &trans
		, tlm::tlm_phase &phase
		, sc_time &delay
		);
	
		

};
#endif
