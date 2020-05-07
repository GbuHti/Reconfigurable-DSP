#include "master.h"
#include "crossbar.h"

int sc_main(int argc, char *argv[])
{
	sc_time clock(10, SC_NS);
	
	Crossbar crossbar("crossbar");

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

////<=========== MASTER & CROSSBAR =========================================
	master0.isock.bind(crossbar.tsock[0]);
	master1.isock.bind(crossbar.tsock[1]);
	master2.isock.bind(crossbar.tsock[2]);
	master3.isock.bind(crossbar.tsock[3]);
	master4.isock.bind(crossbar.tsock[4]);
	master5.isock.bind(crossbar.tsock[5]);
	master6.isock.bind(crossbar.tsock[6]);
	master7.isock.bind(crossbar.tsock[7]);
	master8.isock.bind(crossbar.tsock[8]);
	master9.isock.bind(crossbar.tsock[9]);
	master10.isock.bind(crossbar.tsock[10]);
	master11.isock.bind(crossbar.tsock[11]);

///<============ CROSSBAR & SLAVER ===========================================
	crossbar.isock[0](slaver_port0.tsock);	
	crossbar.isock[1](slaver_port1.tsock);	
	crossbar.isock[2](slaver_port2.tsock);	
	crossbar.isock[3](slaver_port3.tsock);	
	crossbar.isock[4](slaver_port4.tsock);	
	crossbar.isock[5](slaver_port5.tsock);	
	crossbar.isock[6](slaver_port6.tsock);	
	crossbar.isock[7](slaver_port7.tsock);	
	crossbar.isock[8](slaver_port8.tsock);	
	crossbar.isock[9](slaver_port9.tsock);	
	crossbar.isock[10](slaver_port10.tsock);	
	crossbar.isock[11](slaver_port11.tsock);	
	crossbar.isock[12](slaver_port12.tsock);	
	crossbar.isock[13](slaver_port13.tsock);	
	crossbar.isock[14](slaver_port14.tsock);	
	crossbar.isock[15](slaver_port15.tsock);	


	sc_start(100, SC_NS);
	return 0;
}
