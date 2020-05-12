#ifndef CROSSBAR_H
#define CROSSBAR_H

#include "systemc.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include "contextreg_if.h"
#include "define.h"
#include "distributor.h"
#include "combiner.h"

#include <iostream>
#include <map>

using namespace std;
using namespace sc_core;

/**
 * @note:
 * 当PE的一路输入接收来自自己的数据时，需要crossbar在初始化全部完成之后
 * 发起一个激活transaction
 */
class Crossbar : public sc_module
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
		tlm::tlm_initiator_socket<> isock[2*ARITH_PE_NUM + STORER_NUM];

		Crossbar
		( sc_module_name name
		);

		void write_context_reg(slc context) override;
		void all_config() override;
		 
	private:
		Distributor distributor0;
		Distributor distributor1;
		Distributor distributor2;
		Distributor distributor3;
		Distributor distributor4;
		Distributor distributor5;
		Distributor distributor6;
		Distributor distributor7;
		Distributor distributor8;
		Distributor distributor9;
		Distributor distributor10;
		Distributor distributor11;

		Combiner combiner0;
		Combiner combiner1;
		Combiner combiner2;
		Combiner combiner3;
		Combiner combiner4;
		Combiner combiner5;
		Combiner combiner6;
		Combiner combiner7;
		Combiner combiner8;
		Combiner combiner9;
		Combiner combiner10;
		Combiner combiner11;
		Combiner combiner12;
		Combiner combiner13;
		Combiner combiner14;
		Combiner combiner15;
		Combiner combiner16;
		Combiner combiner17;

		contextreg_if *config_holder[30];
};


#endif
