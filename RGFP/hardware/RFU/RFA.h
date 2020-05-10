#ifndef RFA_H
#define RFA_H

#include "systemc.h"
#include "tlm.h"
#include "loader.h"
#include "crossbar.h"
#include "contextreg_if.h"
#include "tlm_utils/simple_target_socket.h"

class RFA : public sc_core::sc_module
			, virtual public tlm::tlm_bw_transport_if<>
					 , virtual public tlm::tlm_fw_transport_if<>
{
	public:
		RFA
		( sc_core::sc_module_name	name
		);

	private:
		/// Not Implemented for this example but required by the initiator socket
		void												
		invalidate_direct_mem_ptr					
		( sc_dt::uint64      start_range                        
		, sc_dt::uint64      end_range                 
		);

		/// Not Implemented for this example but require by the initiator socket
		tlm::tlm_sync_enum	 
		nb_transport_bw								
		( tlm::tlm_generic_payload  &payload
		, tlm::tlm_phase            &phase
		, sc_core::sc_time          &delta
		);


	public:
		tlm::tlm_initiator_socket<> initiator_socket_0;
		tlm::tlm_initiator_socket<> initiator_socket_1;
		tlm::tlm_initiator_socket<> initiator_socket_2;
		tlm::tlm_initiator_socket<> initiator_socket_3;

		tlm::tlm_target_socket<>	target_sokcet;

	public:
		///< 
		tlm::tlm_sync_enum	 
		nb_transport_fw								
		( tlm::tlm_generic_payload  &payload
		, tlm::tlm_phase            &phase
		, sc_core::sc_time          &delta
		);

	private:
		const sc_core::sc_time loader_end_rsp_delay = sc_core::SC_ZERO_TIME;
		const unsigned	PE_NUM						= 14;

		Loader m_loader_0;
		Loader m_loader_1;
		Loader m_loader_2;
		Loader m_loader_3;

		contextreg_if * m_PE_proxy[14];
};

#endif
