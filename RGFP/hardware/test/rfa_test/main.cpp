#include "master.h"
#include "RFA.h"
#include "device_proxy.h"
#include "memory.h"
#include "data_loader.h"

#define SIZE  1024*4*32

int sc_main(int argc, char *argv[])
{
	assert(argc == 2);
	sc_time clock(10, SC_NS);

	Config config("config",1);
	Data_loader data_loader(argv[1]);
	
	RFA rfa("rfa", &config);

	Memory memory("memory", SIZE, SC_ZERO_TIME, SC_ZERO_TIME);	//32K字

	DeviceProxy deviceproxy0("deviceproxy0", SC_ZERO_TIME, &memory);
	DeviceProxy deviceproxy1("deviceproxy1", SC_ZERO_TIME, &memory);
	DeviceProxy deviceproxy2("deviceproxy2", SC_ZERO_TIME, &memory);
	DeviceProxy deviceproxy3("deviceproxy3", SC_ZERO_TIME, &memory);
	DeviceProxy deviceproxy4("deviceproxy4", SC_ZERO_TIME, &memory);
	DeviceProxy deviceproxy5("deviceproxy5", SC_ZERO_TIME, &memory);

	rfa.isock[0].bind(deviceproxy0.tsock);
	rfa.isock[1].bind(deviceproxy1.tsock);
	rfa.isock[2].bind(deviceproxy2.tsock);
	rfa.isock[3].bind(deviceproxy3.tsock);
	rfa.isock[4].bind(deviceproxy4.tsock);
	rfa.isock[5].bind(deviceproxy5.tsock);

	config.config[0] = &rfa;

	data_loader.load_data(memory.m_data, memory.m_size);
	
	sc_start();

	data_loader.extract_data(memory.m_data, memory.m_size);
	return 0;
}
