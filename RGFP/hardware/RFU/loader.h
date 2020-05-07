#ifndef LOADER_H
#define LOADER_H

#include "contextreg_if.h"
#include "slcs_if.h"

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include <queue>
#include "define.h"

class Loader:public sc_core::sc_module
			 ,public contextreg_if
{
	private:
		unsigned m_ID;
		unsigned m_batch_len;
		unsigned m_addr_inc;
		unsigned m_addr;
		unsigned m_op_aux;

		sc_core::sc_time m_end_rsp_delay;

		unsigned m_data_cnt;
		tlm::tlm_generic_payload m_trans;
		unsigned char m_src_data[4];
		//bool m_src_data_used;
		bool m_all_ini_done;
		
		slcs_if * slcs; ///< the interface between Loader and Reconfiguration Controller

		sc_core::sc_event mAllInitiatedEvent;
		sc_core::sc_event mEndRequestEvent;
		sc_core::sc_event mBeginResponseEvent;
		sc_core::sc_event mUseSourceDataEvent;
		sc_core::sc_event mBeginRequestEvent;
		sc_core::sc_event mCompleteEvent;
		
		
	public:
		tlm_utils::simple_initiator_socket<Loader> isock_memory;
		tlm_utils::simple_initiator_socket<Loader> isock_crossbar;

		Loader
			(
			 sc_core::sc_module_name name
			 ,unsigned ID
			 ,sc_core::sc_time end_rsp_delay
			);

		void read_data_thread();

		void send_thread();

		void write_context_reg(slc context) override;
		void all_config() override;

		tlm::tlm_sync_enum 
		nb_transport_bw_memory
		( tlm::tlm_generic_payload	&trans
		, tlm::tlm_phase			&pahse
		, sc_core::sc_time			&delay
		);

		tlm::tlm_sync_enum 
		nb_transport_bw_crossbar
		( tlm::tlm_generic_payload	&trans
		, tlm::tlm_phase			&pahse
		, sc_core::sc_time			&delay
		);




};

#endif


