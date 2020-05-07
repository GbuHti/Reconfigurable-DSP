#include "crossbar.h"

Crossbar::Crossbar
( sc_module_name name
 
)
: sc_module(name)
{

}

/**
 *
 */
void Crossbar::write_context_reg(slc context)
{
	unsigned a = context.mux_a;
	unsigned b = context.mux_b;
	unsigned phid = context.phid;
	unsigned context_set_index =  phid - ARITH_PE_NUM;
	m_context_set[context_set_index].mux_a = a;
	m_context_set[context_set_index].mux_b = b;
	branch_node_table[a].insert(std::pair<unsigned, unsigned>(a,phid));
	branch_node_table[b].insert(std::pair<unsigned, unsigned>(b,phid));
}

/**
 *
 */
void Crossbar::mux_ud_thread_8a()
{
	wait(mBeginRequestEvent);
	tlm::tlm_sync_enum return_value;
	return_value = isock[0].nb_transport_fw();
}
