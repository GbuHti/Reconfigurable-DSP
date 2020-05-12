#include "master.h"
#include "arith_PE.h"

/**
 * @function:
 * 0,multi端口功能测试
 * 1,单次计算
 * 2,多次计算
 * 3,单目计算
 * 4,双目计算
 * 5,不定长延时反馈，计算延迟与理论值对比
 */
int sc_main(int argc, char *argv[])
{
	sc_time clock(10, SC_NS);

	Config config("config",1);
	
	Master master0("master0", 0);
	Master master1("master1", 1);

	Slaver crossbar0("crossbar0", 0, 'a', clock);

	Arith_PE arith_pe("arith_pe", 4, true);

	master0.isock.bind(arith_pe.tsock[0]);
	master1.isock.bind(arith_pe.tsock[1]);
	arith_pe.isock.bind(crossbar0.tsock);

	config.config[0] = &arith_pe;
	arith_pe.slcs = &config;
	
	sc_start(200, SC_NS);
	return 0;
}
