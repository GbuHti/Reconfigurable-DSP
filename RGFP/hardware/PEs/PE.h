#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"

struct first_level_config
{
	union {
		uint32_t reg;
		struct {
			unsigned reserved: 14;
			unsigned fsm_en:	1;
			unsigned mux_b:		5;
			unsigned mux_a:		5;
			unsigned op:		3;
			unsigned cb_id:		4;
		}math_oprt;
		struct {
			unsigned addr_inc:	13;
			unsigned batch_len:	12;	
			unsigned op:		3;
			unsigned cb_id:		4;
		}ldst;
	};	
};


class PE: public sc_module
{
	private:
		first_level_config		m_config_reg;
		uint32_t		m_src_a;
		uint32_t		m_src_b;
		uint32_t		m_result;
				 
	public:
		tlm_utils::simple_target_socket<PE> m_tsock_a;	
		tlm_utils::simple_target_socket<PE> m_tsock_b;
		tlm_utils::simple_initiator_socket<PE> m_isock_result;

		enum op_type
		{
			LOAD	= 0,
			STORE,
			ADD,
			SUB,
			MUL,
			DIV,
			SQRT,
			UNDEF
		};

		PE
		(   sc_core::sc_module_name module_name  ///< SC module name 
		,	const char				*tsock_a
		,	const char				*tsock_b
		,	const char				*isock_result		
		,	first_level_config				config_reg
			 
		);

		void function_sel
		(
		);

		void do_operation
		(
		);	
	
		void b_transport
		( tlm::tlm_generic_payload  &gp
		, sc_core::sc_time			&delay_time
		);	

};

