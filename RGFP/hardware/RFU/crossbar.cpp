#include "crossbar.h"
#include "define.h"

//n号dist的端口连到所有combiner的n号端口
#define CONNECT_DIST_2_COMB(x) \
	distributor ## x.isock[0].bind(combiner0.tsock[x]);\
	distributor ## x.isock[1].bind(combiner1.tsock[x]);\
	distributor ## x.isock[2].bind(combiner2.tsock[x]);\
	distributor ## x.isock[3].bind(combiner3.tsock[x]);\
	distributor ## x.isock[4].bind(combiner4.tsock[x]);\
	distributor ## x.isock[5].bind(combiner5.tsock[x]);\
	distributor ## x.isock[6].bind(combiner6.tsock[x]);\
	distributor ## x.isock[7].bind(combiner7.tsock[x]);\
	distributor ## x.isock[8].bind(combiner8.tsock[x]);\
	distributor ## x.isock[9].bind(combiner9.tsock[x]);\
	distributor ## x.isock[10].bind(combiner10.tsock[x]);\
	distributor ## x.isock[11].bind(combiner11.tsock[x]);\
	distributor ## x.isock[12].bind(combiner12.tsock[x]);\
	distributor ## x.isock[13].bind(combiner13.tsock[x]);\
	distributor ## x.isock[14].bind(combiner14.tsock[x]);\
	distributor ## x.isock[15].bind(combiner15.tsock[x]);\
	distributor ## x.isock[16].bind(combiner16.tsock[x]);\
	distributor ## x.isock[17].bind(combiner17.tsock[x]);\


Crossbar::Crossbar
( sc_module_name name
)
: sc_module(name)
, distributor0("distributor0",0)
, distributor1("distributor1",1)
, distributor2("distributor2",2)
, distributor3("distributor3",3)
, distributor4("distributor4",4)
, distributor5("distributor5",5)
, distributor6("distributor6",6)
, distributor7("distributor7",7)
, distributor8("distributor8",8)
, distributor9("distributor9",9)
, distributor10("distributor10",10)
, distributor11("distributor11",11)
, combiner0("combiner0", 0, SC_ZERO_TIME)
, combiner1("combiner1", 1, SC_ZERO_TIME)
, combiner2("combiner2", 2, SC_ZERO_TIME)
, combiner3("combiner3", 3, SC_ZERO_TIME)
, combiner4("combiner4", 4, SC_ZERO_TIME)
, combiner5("combiner5", 5, SC_ZERO_TIME)
, combiner6("combiner6", 6, SC_ZERO_TIME)
, combiner7("combiner7", 7, SC_ZERO_TIME)
, combiner8("combiner8", 8, SC_ZERO_TIME)
, combiner9("combiner9", 9, SC_ZERO_TIME)
, combiner10("combiner10", 10, SC_ZERO_TIME)
, combiner11("combiner11", 11, SC_ZERO_TIME)
, combiner12("combiner12", 12, SC_ZERO_TIME)
, combiner13("combiner13", 13, SC_ZERO_TIME)
, combiner14("combiner14", 14, SC_ZERO_TIME)
, combiner15("combiner15", 15, SC_ZERO_TIME)
, combiner16("combiner16", 16, SC_ZERO_TIME)
, combiner17("combiner17", 17, SC_ZERO_TIME)
{
////<=========== INPUT & DISTRIBUTOR =========================================
	tsock[0](distributor0.tsock);
	tsock[1](distributor1.tsock);
	tsock[2](distributor2.tsock);
	tsock[3](distributor3.tsock);
	tsock[4](distributor4.tsock);
	tsock[5](distributor5.tsock);
	tsock[6](distributor6.tsock);
	tsock[7](distributor7.tsock);
	tsock[8](distributor8.tsock);
	tsock[9](distributor9.tsock);
	tsock[10](distributor10.tsock);
	tsock[11](distributor11.tsock);

///<============ DISTRIBUTOR & COMBINER CONNETC===============================
	CONNECT_DIST_2_COMB(0);
	CONNECT_DIST_2_COMB(1);
	CONNECT_DIST_2_COMB(2);
	CONNECT_DIST_2_COMB(3);
	CONNECT_DIST_2_COMB(4);
	CONNECT_DIST_2_COMB(5);
	CONNECT_DIST_2_COMB(6);
	CONNECT_DIST_2_COMB(7);
	CONNECT_DIST_2_COMB(8);
	CONNECT_DIST_2_COMB(9);
	CONNECT_DIST_2_COMB(10);
	CONNECT_DIST_2_COMB(11);

///<============ COMBINER & SLAVER ===========================================
	combiner0.isock.bind(isock[0]);	
	combiner1.isock.bind(isock[1]);	
	combiner2.isock.bind(isock[2]);	
	combiner3.isock.bind(isock[3]);	
	combiner4.isock.bind(isock[4]);	
	combiner5.isock.bind(isock[5]);	
	combiner6.isock.bind(isock[6]);	
	combiner7.isock.bind(isock[7]);	
	combiner8.isock.bind(isock[8]);	
	combiner9.isock.bind(isock[9]);	
	combiner10.isock.bind(isock[10]);	
	combiner11.isock.bind(isock[11]);	
	combiner12.isock.bind(isock[12]);	
	combiner13.isock.bind(isock[13]);	
	combiner14.isock.bind(isock[14]);	
	combiner15.isock.bind(isock[15]);	
	combiner16.isock.bind(isock[16]);
	combiner17.isock.bind(isock[17]);	

	config_holder[0] = &distributor0;
	config_holder[1] = &distributor1;
	config_holder[2] = &distributor2;
	config_holder[3] = &distributor3;
	config_holder[4] = &distributor4;
	config_holder[5] = &distributor5;
	config_holder[6] = &distributor6;
	config_holder[7] = &distributor7;
	config_holder[8] = &distributor8;
	config_holder[9] = &distributor9;
	config_holder[10] = &distributor10;
	config_holder[11] = &distributor11;
	config_holder[12] = &combiner0;
	config_holder[13] = &combiner1;
	config_holder[14] = &combiner2;
	config_holder[15] = &combiner3;
	config_holder[16] = &combiner4;
	config_holder[17] = &combiner5;
	config_holder[18] = &combiner6;
	config_holder[19] = &combiner7;
	config_holder[20] = &combiner8;
	config_holder[21] = &combiner9;
	config_holder[22] = &combiner10;
	config_holder[23] = &combiner11;
	config_holder[24] = &combiner12;
	config_holder[25] = &combiner13;
	config_holder[26] = &combiner14;
	config_holder[27] = &combiner15;
	config_holder[28] = &combiner16;
	config_holder[29] = &combiner17;
}

//Broastcast to sub-modules
void Crossbar::write_context_reg(slc context)
{
	for(unsigned i=0;i < (LOADER_NUM+3*ARITH_PE_NUM+STORER_NUM); i++)
	{
		config_holder[i]->write_context_reg(context);	
	}
}
void Crossbar::all_config()
{
	for(unsigned i=0;i < (LOADER_NUM+3*ARITH_PE_NUM+STORER_NUM); i++)
	{
		config_holder[i]->all_config();
	}
}

void												
Crossbar::invalidate_direct_mem_ptr					
( sc_dt::uint64      start_range                        
  , sc_dt::uint64      end_range                 
  )
{
	
}

/// Not Implemented for this example but require by the initiator socket
tlm::tlm_sync_enum	 
Crossbar::nb_transport_bw								
( tlm::tlm_generic_payload  &payload
  , tlm::tlm_phase            &phase
  , sc_core::sc_time          &delta
  )
{
	assert(false && "not implenmented");	
	return tlm::TLM_COMPLETED;
}

tlm::tlm_sync_enum
Crossbar::nb_transport_fw								
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
Crossbar::b_transport
( tlm::tlm_generic_payload  &payload                // ref to payload 
  , sc_core::sc_time          &delay_time             // delay time 
  )
{
	assert(false && "not implenmented");	
}


/// Not implemented for this example but required by interface
bool                                              // success / failure
Crossbar::get_direct_mem_ptr                       
( tlm::tlm_generic_payload   &payload,            // address + extensions
  tlm::tlm_dmi               &dmi_data            // DMI data
  )
{
	assert(false && "not implenmented");	
	return false;
}


/// Not implemented for this example but required by interface
unsigned int                                      // result
Crossbar::transport_dbg                            
( tlm::tlm_generic_payload  &payload              // debug payload
)
{
	assert(false && "not implenmented");	
	return 0;
}

