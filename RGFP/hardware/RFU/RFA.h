#ifndef RFA_H
#define RFA_H

#include "systemc.h"
#include "tlm.h"
#include "loader.h"
#include "crossbar.h"
#include "arith_PE.h"
#include "storer.h"
#include "contextreg_if.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"

class RFA : public sc_core::sc_module
		  , virtual public tlm::tlm_bw_transport_if<>
		  , virtual public tlm::tlm_fw_transport_if<>
		  , public contextreg_if
{
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

		tlm::tlm_sync_enum
			nb_transport_fw								
			( tlm::tlm_generic_payload  &payload
			  , tlm::tlm_phase            &phase
			  , sc_core::sc_time          &delta
			);

		/// b_transport() - Blocking Transport
		void                                                // returns nothing 
			b_transport
			( tlm::tlm_generic_payload  &payload                // ref to payload 
			  , sc_core::sc_time          &delay_time             // delay time 
			);


		/// Not implemented for this example but required by interface
		bool                                              // success / failure
			get_direct_mem_ptr                       
			( tlm::tlm_generic_payload   &payload,            // address + extensions
			  tlm::tlm_dmi               &dmi_data            // DMI data
			);


		/// Not implemented for this example but required by interface
		unsigned int                                      // result
			transport_dbg                            
			( tlm::tlm_generic_payload  &payload              // debug payload
			);


	public:
		tlm::tlm_target_socket<> tsock[LOADER_NUM+ARITH_PE_NUM];
		tlm::tlm_initiator_socket<> isock[LOADER_NUM + STORER_NUM];

	public:
		RFA
		( sc_core::sc_module_name	name
		);

		void write_context_reg(slc context) override;
		void all_config() override;

	private:
		const sc_core::sc_time loader_end_rsp_delay = sc_core::SC_ZERO_TIME;

		Loader m_loader_0;
		Loader m_loader_1;
		Loader m_loader_2;
		Loader m_loader_3;

		Crossbar m_crossbar;

		Arith_PE m_addsub_0;
		Arith_PE m_addsub_1;		
		Arith_PE m_addsub_2;
		Arith_PE m_addsub_3;		
		Arith_PE m_muldiv_0;		
		Arith_PE m_muldiv_1;		
		Arith_PE m_sqrt_0;		
		Arith_PE m_sincos_0;

		Storer m_storer_0;
		Storer m_storer_1;

		contextreg_if * config_holder[LOADER_NUM+ARITH_PE_NUM+LOADER_NUM+1];
};

#endif
