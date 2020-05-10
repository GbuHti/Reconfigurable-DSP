#include "master.h"
#include "storer.h"
#include "device_proxy.h"
#include "memory.h"


class Slcs : public slcs_if
{
	public:
		void release_busy()
		{
		}
};

int sc_main(int argc, char *argv[])
{
	sc_time clock(10, SC_NS);

	Config config("config",1);
	
	Master master0("master0", 0);

	Storer storer("storer", 12);

	Memory memory("memory", 1024*4*32, SC_ZERO_TIME, SC_ZERO_TIME);	

	DeviceProxy deviceproxy("deviceproxy", SC_ZERO_TIME, &memory);

	master0.isock.bind(storer.tsock);
	storer.isock.bind(deviceproxy.tsock);


	storer.slcs = &config;
	config.config[0] = &storer;
	
	sc_start(200, SC_NS);
	return 0;
}
