#include "ReconfigController.h"
#include "RFA.h"

RFA::RFA
( sc_core::sc_module_name name
)
:sc_module (name)

, m_loader_0
( "m_loader_0"
, 0
, loader_end_rsp_delay
)

, m_loader_1
( "m_loader_1"
, 1
, loader_end_rsp_delay
)

, m_loader_2
( "m_loader_2"
, 2
, loader_end_rsp_delay
)

, m_loader_3
( "m_loader_3"
, 3
, loader_end_rsp_delay
)
{
	m_loader_0.isock_memory(initiator_socket_0);
	m_loader_1.isock_memory(initiator_socket_1);
	m_loader_2.isock_memory(initiator_socket_2);
	m_loader_3.isock_memory(initiator_socket_3);
	
	m_PE_proxy[0] = &m_loader_0;
	m_PE_proxy[1] = &m_loader_1;
	m_PE_proxy[2] = &m_loader_2;
	m_PE_proxy[3] = &m_loader_3;
}

tlm::tlm_sync_enum	 
RFA::nb_transport_fw								
( tlm::tlm_generic_payload  &trans
, tlm::tlm_phase            &phase
, sc_core::sc_time          &delay
)
{
	slc context = *(slc *)trans.get_data_ptr();
	
	m_PE_proxy[context.phid]->write_context_reg(context);
	delay = sc_core::SC_ZERO_TIME;

	return tlm::TLM_COMPLETED; //only 1 phase supported
}


