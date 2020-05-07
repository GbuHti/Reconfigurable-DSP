#include "master.h"
#include "distributor.h"
#include "combiner.h"

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


int sc_main(int argc, char *argv[])
{
	sc_time clock(10, SC_NS);
	
	Config config("config",28);

	Master master0("master0", 0);
	Master master1("master1", 1);
	Master master2("master2", 2);
	Master master3("master3", 3);
	Master master4("master4", 4);
	Master master5("master5", 5);
	Master master6("master6", 6);
	Master master7("master7", 7);
	Master master8("master8", 8);
	Master master9("master9", 9);
	Master master10("master10", 10);
	Master master11("master11", 11);

	Distributor distributor0("distributor0",0);
	Distributor distributor1("distributor1",1);
	Distributor distributor2("distributor2",2);
	Distributor distributor3("distributor3",3);
	Distributor distributor4("distributor4",4);
	Distributor distributor5("distributor5",5);
	Distributor distributor6("distributor6",6);
	Distributor distributor7("distributor7",7);
	Distributor distributor8("distributor8",8);
	Distributor distributor9("distributor9",9);
	Distributor distributor10("distributor10",10);
	Distributor distributor11("distributor11",11);

	Combiner combiner0("combiner0", 0, SC_ZERO_TIME);
	Combiner combiner1("combiner1", 1, SC_ZERO_TIME);
	Combiner combiner2("combiner2", 2, SC_ZERO_TIME);
	Combiner combiner3("combiner3", 3, SC_ZERO_TIME);
	Combiner combiner4("combiner4", 4, SC_ZERO_TIME);
	Combiner combiner5("combiner5", 5, SC_ZERO_TIME);
	Combiner combiner6("combiner6", 6, SC_ZERO_TIME);
	Combiner combiner7("combiner7", 7, SC_ZERO_TIME);
	Combiner combiner8("combiner8", 8, SC_ZERO_TIME);
	Combiner combiner9("combiner9", 9, SC_ZERO_TIME);
	Combiner combiner10("combiner10", 10, SC_ZERO_TIME);
	Combiner combiner11("combiner11", 11, SC_ZERO_TIME);
	Combiner combiner12("combiner12", 12, SC_ZERO_TIME);
	Combiner combiner13("combiner13", 13, SC_ZERO_TIME);
	Combiner combiner14("combiner14", 14, SC_ZERO_TIME);
	Combiner combiner15("combiner15", 15, SC_ZERO_TIME);

	Slaver slaver_port0("slaver_port0", 4, 'a', clock);
	Slaver slaver_port1("slaver_port1", 4, 'b', clock);
	Slaver slaver_port2("slaver_port2", 5, 'a', clock);
	Slaver slaver_port3("slaver_port3", 5, 'b', clock);
	Slaver slaver_port4("slaver_port4", 6, 'a', clock);
	Slaver slaver_port5("slaver_port5", 6, 'b', clock);
	Slaver slaver_port6("slaver_port6", 7, 'a', clock);
	Slaver slaver_port7("slaver_port7", 7, 'b', clock);
	Slaver slaver_port8("slaver_port0", 8, 'a', clock);
	Slaver slaver_port9("slaver_port8", 8, 'b', clock);
	Slaver slaver_port10("slaver_port9", 9, 'a', clock);
	Slaver slaver_port11("slaver_port10", 9, 'b', clock);
	Slaver slaver_port12("slaver_port11", 10, 'a', clock);
	Slaver slaver_port13("slaver_port12", 10, 'b', clock);
	Slaver slaver_port14("slaver_port13", 11, 'a', clock);
	Slaver slaver_port15("slaver_port14", 11, 'b', clock);


////<=========== MASTER & DISTRIBUTOR =========================================
	master0.isock.bind(distributor0.tsock);
	master1.isock.bind(distributor1.tsock);
	master2.isock.bind(distributor2.tsock);
	master3.isock.bind(distributor3.tsock);
	master4.isock.bind(distributor4.tsock);
	master5.isock.bind(distributor5.tsock);
	master6.isock.bind(distributor6.tsock);
	master7.isock.bind(distributor7.tsock);
	master8.isock.bind(distributor8.tsock);
	master9.isock.bind(distributor9.tsock);
	master10.isock.bind(distributor10.tsock);
	master11.isock.bind(distributor11.tsock);

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
	combiner0.isock.bind(slaver_port0.tsock);	
	combiner1.isock.bind(slaver_port1.tsock);	
	combiner2.isock.bind(slaver_port2.tsock);	
	combiner3.isock.bind(slaver_port3.tsock);	
	combiner4.isock.bind(slaver_port4.tsock);	
	combiner5.isock.bind(slaver_port5.tsock);	
	combiner6.isock.bind(slaver_port6.tsock);	
	combiner7.isock.bind(slaver_port7.tsock);	
	combiner8.isock.bind(slaver_port8.tsock);	
	combiner9.isock.bind(slaver_port9.tsock);	
	combiner10.isock.bind(slaver_port10.tsock);	
	combiner11.isock.bind(slaver_port11.tsock);	
	combiner12.isock.bind(slaver_port12.tsock);	
	combiner13.isock.bind(slaver_port13.tsock);	
	combiner14.isock.bind(slaver_port14.tsock);	
	combiner15.isock.bind(slaver_port15.tsock);	


	config.config[0] = &distributor0;
	config.config[1] = &distributor1;
	config.config[2] = &distributor2;
	config.config[3] = &distributor3;
	config.config[4] = &distributor4;
	config.config[5] = &distributor5;
	config.config[6] = &distributor6;
	config.config[7] = &distributor7;
	config.config[8] = &distributor8;
	config.config[9] = &distributor9;
	config.config[10] = &distributor10;
	config.config[11] = &distributor11;
	config.config[12] = &combiner0;
	config.config[13] = &combiner1;
	config.config[14] = &combiner2;
	config.config[15] = &combiner3;
	config.config[16] = &combiner4;
	config.config[17] = &combiner5;
	config.config[18] = &combiner6;
	config.config[19] = &combiner7;
	config.config[20] = &combiner8;
	config.config[21] = &combiner9;
	config.config[22] = &combiner10;
	config.config[23] = &combiner11;
	config.config[24] = &combiner12;
	config.config[25] = &combiner13;
	config.config[26] = &combiner14;
	config.config[27] = &combiner15;

	sc_start(100, SC_NS);
	return 0;
}
