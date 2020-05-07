#include "combiner.h"

SC_HAS_PROCESS(Combiner);
Combiner::Combiner
( sc_module_name name
, unsigned portid
, sc_time accept_delay
)
: sc_module (name)
, m_portID(portid)
, m_ini_done(false)
, m_accept_delay(accept_delay)
, mBeginRequestPEQ("mBeginRequestPEQ")
, mBeginResponsePEQ("mBeginResponsePEQ")
{
	for(int i = 0; i < (LOADER_NUM+ARITH_PE_NUM); i++)
	{
		tsock[i].register_nb_transport_fw(this, &Combiner::nb_transport_fw, i);	
	}
	isock.register_nb_transport_bw(this, &Combiner::nb_transport_bw);

	SC_THREAD(request_thread);
	SC_THREAD(response_thread);
}

void Combiner::write_context_reg(slc context)
{
//	assert((m_portID/2 + LOADER_NUM) == context.phid); //采用广播模式下发context，此语句无用

	if((m_portID/2 + LOADER_NUM) == context.phid)
	{
		if(m_portID%2 == 0) 
			m_select = context.mux_a;
		else 
			m_select = context.mux_b;
	}
}

void Combiner::all_config()
{
	m_ini_done = true;
}

tlm::tlm_sync_enum 
Combiner::nb_transport_fw
( int id
, tlm::tlm_generic_payload &trans
, tlm::tlm_phase &phase
, sc_time &delay
)
{
	assert(phase == tlm::BEGIN_REQ);
	if(id == (int)m_select)
		mBeginRequestPEQ.notify(trans, delay);

	return tlm::TLM_ACCEPTED;
}

void Combiner::request_thread()
{
	while(true)
	{
		wait(mBeginRequestPEQ.get_event());
		tlm::tlm_generic_payload * trans_ptr;
		while((trans_ptr = mBeginRequestPEQ.get_next_transaction()) != 0)
		{
			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			sc_time delay = SC_ZERO_TIME;
			tlm::tlm_sync_enum return_value;

			return_value = isock->nb_transport_fw(*trans_ptr, phase, delay);	
			assert(return_value == tlm::TLM_ACCEPTED);	
		}
	}
}


tlm::tlm_sync_enum 
Combiner::nb_transport_bw
( tlm::tlm_generic_payload &trans
, tlm::tlm_phase &phase
, sc_time &delay
)
{
	assert( phase == tlm::BEGIN_RESP);
	mBeginResponsePEQ.notify(trans, delay);

	return tlm::TLM_ACCEPTED;
}

void Combiner::response_thread()
{
	while(true)
	{
		wait(mBeginResponsePEQ.get_event());
		tlm::tlm_generic_payload * trans_ptr;
		while((trans_ptr = mBeginResponsePEQ.get_next_transaction()) != 0)
		{
			tlm::tlm_phase phase = tlm::BEGIN_RESP;
			sc_time delay = SC_ZERO_TIME;
			tlm::tlm_sync_enum return_value;
			return_value = tsock[m_select]->nb_transport_bw(*trans_ptr, phase, delay);	
			assert(return_value == tlm::TLM_ACCEPTED);
		}	
	}
}





















