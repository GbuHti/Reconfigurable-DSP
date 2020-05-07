#ifndef CROSSBAR_H
#define CROSSBAR_H

#include "systemc.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include "contextreg_if.h"

#include <iostream>
#include <map>

using namespace std;

#define PE_NUM 14
#define ARITH_PE_NUM 8

#define mux_ud(portid) \
	void mux_ud_thread_ ## portid () 

struct context
{
	unsigned mux_a;
	unsigned mux_b;
};

class Crossbar : public sc_core::sc_module
			   , public contextreg_if
{
	private:
		context m_context_set[ARITH_PE_NUM];		
		array < multimap<unsigned, unsigned>, 12> branch_node_table;
		sc_event mBeginRequestEvent; ///< PE结果发送到crossbar
		
	public:
		tlm_utils::simple_target_socket_tagged<Crossbar> tsock[PE_NUM];
		tlm_utils::simple_initiator_socket_tagged<Crossbar> isock[2*PE_NUM];

	public:
		Crossbar
		( sc_core::sc_module_name name
		);

		void write_context_reg(slc context) override;

		void mux_ud_thread_8a();
		void mux_ud_thread_8b();
		void mux_ud_thread_9a();
		void mux_ud_thread_9b();
		void mux_ud_thread_10a();
		void mux_ud_thread_10b();
		void mux_ud_thread_11a();
		void mux_ud_thread_11b();
		void mux_ud_thread_12a();
		void mux_ud_thread_12b();
		void mux_ud_thread_13a();
		void mux_ud_thread_13b();
		void mux_ud_thread_14a();
		void mux_ud_thread_14b();
		void mux_ud_thread_15a();
		void mux_ud_thread_15b();

		void mux_du_thread_0();
		void mux_du_thread_1();
		void mux_du_thread_2();
		void mux_du_thread_3();
		void mux_du_thread_4();
		void mux_du_thread_5();
		void mux_du_thread_6();
		void mux_du_thread_7();
		void mux_du_thread_8();
		void mux_du_thread_9();
		void mux_du_thread_10();
		void mux_du_thread_11();

		tlm::tlm_sync_enum
		nb_transport_fw
		( int id
		, tlm::tlm_generic_payload  &trans
		, tlm::tlm_phase			&phase
		, sc_core::sc_time			&delay
		);

		tlm::tlm_sync_enum
		nb_transport_bw
		( int id
		, tlm::tlm_generic_payload	&trans
		, tlm::tlm_phase			&phase
		, sc_core::sc_time			&delay
		);
};

#endif
