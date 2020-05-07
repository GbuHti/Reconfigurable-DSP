#ifndef COMBINER_H
#define COMBINER_H

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_get.h"

#include "define.h"
#include "contextreg_if.h"

using namespace sc_core;
using namespace std;

/**
 * @brief
 * @function 每当输入单口发起一次交易，根据context register从多个输入端口选择一个transaction发送到输出端口
 */

class Combiner : public sc_module
			   , public contextreg_if
{
	private:
		unsigned m_portID;
		unsigned m_select;
		bool m_ini_done;
		sc_time m_accept_delay;

		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mBeginRequestPEQ;	
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mBeginResponsePEQ;	
	
	public:
		tlm_utils::simple_target_socket_tagged<Combiner> tsock[LOADER_NUM+ARITH_PE_NUM];
		tlm_utils::simple_initiator_socket<Combiner> isock;

		Combiner
		( sc_module_name name
		, unsigned portid
		, sc_time accept_delay
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
		
		void request_thread();	

		void response_thread();

		void write_context_reg(slc context) override;

		void all_config() override;

};

#endif
