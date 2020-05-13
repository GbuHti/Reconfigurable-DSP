#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_get.h"

#include "define.h"
#include "contextreg_if.h"
/**
 * @brief
 * @function 每当端口发起一次传输交易，该模块根据内部context寄存器将交易分发到指定
 * 的端口
 */
using namespace sc_core;
using namespace std;

struct flags
{
	bool selected; ///< indicate which output port is used in this configuration
	bool hasSend; ///< indicate weather the selected output port is used to send transaction req or not
	bool hasReceive; ///< indicate weather the selected output port is used to receive transaction resp or not
};
class Distributor : public sc_module
				  , public contextreg_if
{
	private:
		unsigned m_ID;
		array<flags, 2*ARITH_PE_NUM+LOADER_NUM>  m_branch_tb;
		bool m_ready;
		bool m_ini_done;
		
		sc_event mTransArrivedEvent;
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mBeginRequestPEQ;	
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mBeginResponsePEQ;	
	
		sc_event mSelfStartingEvent;
		tlm::tlm_generic_payload m_self_starting_trans;
		unsigned m_data;
		unsigned m_pending_portid;
		bool	m_isSelfFeeding;

	public:
		tlm_utils::simple_target_socket<Distributor> tsock;
		tlm_utils::simple_initiator_socket_tagged<Distributor> isock[2*ARITH_PE_NUM + STORER_NUM];

		Distributor
		( sc_module_name name
		, unsigned id	  
		);

		tlm::tlm_sync_enum 
		nb_transport_fw
		( tlm::tlm_generic_payload &trans
		, tlm::tlm_phase &phase
		, sc_time &delay
		);

		tlm::tlm_sync_enum 
		nb_transport_bw
		( int id
		, tlm::tlm_generic_payload &trans
		, tlm::tlm_phase &phase
		, sc_time &delay
		);

		void request_thread();	

		void response_thread();

		void self_starting_thread();

		void write_context_reg(slc context) override;

		void all_config() override;

		//helper
		bool isArith(slc context)
		{return context.op == CONFIG_ADD | context.op == CONFIG_MUL | context.op == CONFIG_SQRT | context.op == CONFIG_SIN;}

		bool isStore(slc context)
		{return context.op == CONFIG_STORE;}

};

#endif
