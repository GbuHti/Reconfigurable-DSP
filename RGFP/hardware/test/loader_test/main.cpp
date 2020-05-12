#include "master.h"
#include "device_proxy.h"
#include "memory.h"
#include "data_loader.h"
#include "arith_PE.h"
#include "loader.h"
#include "storer.h"


#define SIZE  1024*4*32
class Slcs : public slcs_if
{
	public:
		void release_busy()
		{
		}
};

int sc_main(int argc, char *argv[])
{
	assert(argc == 2);
	sc_time clock(10, SC_NS);

	Config config("config",4);
	Data_loader data_loader(argv[1]);
	
	Loader loader0("loader0", 0, SC_ZERO_TIME);
	Loader loader1("loader1", 1, SC_ZERO_TIME);

	Arith_PE addsub0("addsub0", 2, true);

	Storer storer0("storer0", 3);

	Memory memory("memory", SIZE, SC_ZERO_TIME, SC_ZERO_TIME);	//32K字

	DeviceProxy deviceproxy0("deviceproxy0", SC_ZERO_TIME, &memory);
	DeviceProxy deviceproxy1("deviceproxy1", SC_ZERO_TIME, &memory);
	DeviceProxy deviceproxy2("deviceproxy2", SC_ZERO_TIME, &memory);

	loader0.isock_memory.bind(deviceproxy0.tsock);
	loader1.isock_memory.bind(deviceproxy1.tsock);
	loader0.isock_crossbar.bind(addsub0.tsock[0]);
	loader1.isock_crossbar.bind(addsub0.tsock[1]);
	addsub0.isock.bind(storer0.tsock);
	storer0.isock.bind(deviceproxy2.tsock);

	config.config[0] = &loader0;
	config.config[1] = &loader1;
	config.config[2] = &addsub0;
	config.config[3] = &storer0;

	loader0.slcs = &config;
	loader1.slcs = &config;
	addsub0.slcs = &config;
	storer0.slcs = &config;

	data_loader.load_data(memory.m_data, memory.m_size);
	
	sc_start();

	data_loader.extract_data(memory.m_data, memory.m_size);
	return 0;
}
