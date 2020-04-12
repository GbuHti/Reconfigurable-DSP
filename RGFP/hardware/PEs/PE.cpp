#include "PE.h"
#include <assert.h>

SC_HAS_PROCESS(PE);

PE::PE
(	sc_core::sc_module_name	module_name
,	const char				*tsock_a
,	const char				*tsock_b
,	const char				*isock_result
,	config_info				config_reg
)
:	sc_module				(module_name)
,	m_tsock_a				(tsock_a)
,	m_tsock_b				(tsock_b)
,	m_isock_result			(isock_result)
,	m_config_reg			(config_reg)
{
	m_tsock_a.register_b_transport(this, &PE::b_transport);
	m_tsock_b.register_b_transport(this, &PE::b_transport);	

	SC_THREAD(do_operation);
}

void PE::b_transport
(	tlm::tlm_generic_payload	&gp
,	sc_core::sc_time			&delay_time
)	
{
	tlm::tlm_command	cmd		= gp.get_address();
//	sc_dt::uint64		address = gp.get_address();			
	unsigned char		*data	= gp.get_data_ptr();
	unsigned int		length	= gp.get_data_length();	
	
	if(cmd == tlm::TLM_WRITE_COMMAND)
	{
		m_config_reg.reg = *(uint32_t *)data;	
		assert(m_config_reg.math_oprt.op != LOAD && m_config_reg.math_oprt.op != STORE);
		assert(length == 4);
		switch(m_config_reg.math_oprt.op)
		{
			case ADD:
				do_ADD();
				break;
			case SUB:
				do_SUB();
				break;
			case MUL:
				do_MUL();
				break;
			case DIV:
				do_DIV();
				break;
			case SQRT:
				do_SQRT();
				break;
			default:
				std::cout << "Unsupported op" << std::endl;
				exit(1);
		}
	}
}

void do_operation
( op_type op
, 
 
)
