#include "RFA.h"

SC_HAS_PROCESS(RFA);

RFA::RFA
(
 sc_module_name name
)
:sc_module (name)
{
	tsock.register_nb_transport_fw(this, &RFA::nb_transport_fw);
}


tlm::tlm_sync_enum RFA::nb_transport_fw
(
 tlm::tlm_generic_payload &trans,
 tlm::tlm_phase &phase,
 sc_core::sc_time &delay		
 )
{
	return tlm::TLM_COMPLETED;
}
