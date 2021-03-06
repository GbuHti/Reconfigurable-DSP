#include "ReconfigController.h"
#include "RFA.h"

RFA::RFA
( sc_core::sc_module_name name
, slcs_if * slcs_ptr
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

, m_crossbar
( "m_crossbar"
)

, m_addsub_0
( "m_addsub_0"
, 4 
, true
)

, m_addsub_1
( "m_addsub_1"
, 5 
, true
)

, m_addsub_2
( "m_addsub_2"
, 6 
, true
)

, m_addsub_3
( "m_addsub_3"
, 7 
, true
)

, m_muldiv_0
( "m_muldiv_0"
, 8
, true
)

, m_muldiv_1
( "m_muldiv_1"
, 9
, true
)

, m_sqrt_0
( "m_sqrt_0"
, 10
, false
)

, m_sincos_0
( "m_sincose_0"
, 11
, false
)

, m_storer_0
( "m_storer_0"
, 12 
)

, m_storer_1
( "m_storer_1"
, 13 
)
{
//================ SUBMODULE & PARENT ===================
	m_loader_0.isock_memory.bind(isock[0]);
	m_loader_1.isock_memory.bind(isock[1]);
	m_loader_2.isock_memory.bind(isock[2]);
	m_loader_3.isock_memory.bind(isock[3]);

	m_storer_0.isock.bind(isock[4]);
	m_storer_1.isock.bind(isock[5]);

//================ MASTERs & CROSSBAR ====================
	m_loader_0.isock_crossbar.bind(m_crossbar.tsock[0]);
	m_loader_1.isock_crossbar.bind(m_crossbar.tsock[1]);
	m_loader_2.isock_crossbar.bind(m_crossbar.tsock[2]);
	m_loader_3.isock_crossbar.bind(m_crossbar.tsock[3]);
	m_addsub_0.isock.bind(m_crossbar.tsock[4]);
	m_addsub_1.isock.bind(m_crossbar.tsock[5]);
	m_addsub_2.isock.bind(m_crossbar.tsock[6]);
	m_addsub_3.isock.bind(m_crossbar.tsock[7]);
	m_muldiv_0.isock.bind(m_crossbar.tsock[8]);
	m_muldiv_1.isock.bind(m_crossbar.tsock[9]);
	m_sqrt_0.isock.bind(m_crossbar.tsock[10]);
	m_sincos_0.isock.bind(m_crossbar.tsock[11]);

//================ CROSSBAR & SLAVERs====================
	m_crossbar.isock[0](m_addsub_0.tsock[0]);
	m_crossbar.isock[1](m_addsub_0.tsock[1]);
	m_crossbar.isock[2](m_addsub_1.tsock[0]);
	m_crossbar.isock[3](m_addsub_1.tsock[1]);
	m_crossbar.isock[4](m_addsub_2.tsock[0]);
	m_crossbar.isock[5](m_addsub_2.tsock[1]);
	m_crossbar.isock[6](m_addsub_3.tsock[0]);
	m_crossbar.isock[7](m_addsub_3.tsock[1]);
	m_crossbar.isock[8](m_muldiv_0.tsock[0]);
	m_crossbar.isock[9](m_muldiv_0.tsock[1]);
	m_crossbar.isock[10](m_muldiv_1.tsock[0]);
	m_crossbar.isock[11](m_muldiv_1.tsock[1]);
	m_crossbar.isock[12](m_sqrt_0.tsock[0]);
	m_crossbar.isock[13](m_sqrt_0.tsock[1]);
	m_crossbar.isock[14](m_sincos_0.tsock[0]);
	m_crossbar.isock[15](m_sincos_0.tsock[1]);
	m_crossbar.isock[16](m_storer_0.tsock);
	m_crossbar.isock[17](m_storer_1.tsock);
	
//------------ RC向RFA发出slc配置信息---------------------
	config_holder[0] = &m_loader_0;
	config_holder[1] = &m_loader_1;
	config_holder[2] = &m_loader_2;
	config_holder[3] = &m_loader_3;
	config_holder[4] = &m_crossbar;
	config_holder[5] = &m_addsub_0 ;
	config_holder[6] = &m_addsub_1 ;
	config_holder[7] = &m_addsub_2 ;
	config_holder[8] = &m_addsub_3 ;
	config_holder[9] = &m_muldiv_0 ;
	config_holder[10] = &m_muldiv_1 ;
	config_holder[11] = &m_sqrt_0 ;
	config_holder[12] = &m_sincos_0 ;
	config_holder[13] = &m_storer_0 ;
	config_holder[14] = &m_storer_1 ;

//RFA向RC发出release_busy信息
	m_loader_0.slcs = slcs_ptr;
	m_loader_1.slcs = slcs_ptr;
	m_loader_2.slcs = slcs_ptr;
	m_loader_3.slcs = slcs_ptr;
	m_addsub_0.slcs = slcs_ptr;
	m_addsub_1.slcs = slcs_ptr;
	m_addsub_2.slcs = slcs_ptr;
	m_addsub_3.slcs = slcs_ptr;
	m_muldiv_0.slcs = slcs_ptr;
	m_muldiv_1.slcs = slcs_ptr;
	m_sqrt_0.slcs = slcs_ptr;
	m_sincos_0.slcs = slcs_ptr;
	m_storer_0.slcs = slcs_ptr;
	m_storer_1.slcs = slcs_ptr;
}

//Broastcast to sub-modules
void RFA::write_context_reg(slc context)
{
	for(unsigned i = 0; i<(LOADER_NUM + ARITH_PE_NUM + STORER_NUM + 1); i++)
	{
		config_holder[i]->write_context_reg(context);	
	}
}
void RFA::all_config()
{
	for(unsigned i = 0; i<(LOADER_NUM+ARITH_PE_NUM+STORER_NUM+1); i++)
	{
		config_holder[i]->all_config();
	}
}

//{{{ 必要的函数
///////////////////////////////////////////////////////////////////////////////////////
void												
RFA::invalidate_direct_mem_ptr					
( sc_dt::uint64      start_range                        
  , sc_dt::uint64      end_range                 
  )
{
	
}

/// Not Implemented for this example but require by the initiator socket
tlm::tlm_sync_enum	 
RFA::nb_transport_bw								
( tlm::tlm_generic_payload  &payload
  , tlm::tlm_phase            &phase
  , sc_core::sc_time          &delta
  )
{
	assert(false && "not implenmented");	
	return tlm::TLM_COMPLETED;
}

tlm::tlm_sync_enum
RFA::nb_transport_fw								
( tlm::tlm_generic_payload  &payload
  , tlm::tlm_phase            &phase
  , sc_core::sc_time          &delta
  )
{
	assert(false && "not implenmented");	
	return tlm::TLM_COMPLETED;
}

/// b_transport() - Blocking Transport
void                                                // returns nothing 
RFA::b_transport
( tlm::tlm_generic_payload  &payload                // ref to payload 
  , sc_core::sc_time          &delay_time             // delay time 
  )
{
	assert(false && "not implenmented");	
}


/// Not implemented for this example but required by interface
bool                                              // success / failure
RFA::get_direct_mem_ptr                       
( tlm::tlm_generic_payload   &payload,            // address + extensions
  tlm::tlm_dmi               &dmi_data            // DMI data
  )
{
	assert(false && "not implenmented");	
	return false;
}


/// Not implemented for this example but required by interface
unsigned int                                      // result
RFA::transport_dbg                            
( tlm::tlm_generic_payload  &payload              // debug payload
)
{
	assert(false && "not implenmented");	
	return 0;
}
///}}}
