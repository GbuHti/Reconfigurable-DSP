#include <cstdlib>
#include <ctime>

#include "iss.h"
#include "memory.h"
#include "elf_loader.h"
#include "clint_if.h"
#include "terminal.h"
#include "bus.h"
#include "device_proxy.h"

#include "ReconfigController.h"
#include "RFA.h"

#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>
#include <boost/io/ios_state.hpp>

#include "define.h"
#include "data_loader.h"

struct Options {
	typedef unsigned int addr_t;

	Options &check_and_post_process()
	{
		mem_end_addr = mem_start_addr + mem_size - 1;
		assert((mem_end_addr < clint_start_addr) && "RAM too big, would overlap memory");	

		return *this;
	}

	std::string input_program;
	std::string input_data;

	addr_t mem_size				= 1024*1024*32; //32 MB ram
	addr_t mem_start_addr		= 0x00000000;
	addr_t mem_end_addr			= mem_start_addr + mem_size - 1;
	addr_t clint_start_addr		= 0x02000000;
	addr_t clint_end_addr		= 0x0200ffff;
	addr_t term_start_addr		= 0x20000000;
	addr_t term_end_addr		= term_start_addr + 16;
	addr_t rfu_start_addr		= 0x30000000;
	addr_t rfu_end_addr			= 0x30000100;

	
	unsigned int tlm_global_quantum = 10;
};

Options parse_command_line_arguments(int argc, char **argv)
{
	try
	{
		Options opt;

		namespace po = boost::program_options ;
		
		po::options_description desc("Options");

		desc.add_options()
			("help", "produce help message")
			("memory-start", po::value<unsigned int>(&opt.mem_start_addr), "set memory start address")
			("tlm_global-quantum", po::value<unsigned int>(&opt.tlm_global_quantum), "set global tlm quantum (in NS)")
			("input-file", po::value<std::string>(&opt.input_program)->required(), "input file to use for execution")
			("input-data", po::value<std::string>(&opt.input_data)->required(), "input data to use for execution")
			;
		po::positional_options_description pos;
		pos.add("input-file", 1);
		pos.add("input-data", 2);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).positional(pos).run(), vm);

		if(vm.count("help"))
		{
			std::cout << desc << std::endl;	
		}

		po::notify(vm);

		return opt.check_and_post_process();
	}catch (boost::program_options::error &e)
	{
		std::cerr << "Error parsing command line options: " << e.what() << std::endl;
		exit(-1);	
	}
}

int sc_main(int argc, char **argv)
{
	Options opt = parse_command_line_arguments(argc, argv);

	std::srand(std::time(nullptr)); // use current time as seed for random generator

	sc_core::sc_time clock = sc_time(10,sc_core::SC_NS);
	unsigned int max_txns = 4;

	ISS core;
	Memory mem("Memory", opt.mem_size, clock, clock);
	DeviceProxy memory_proxy("memory_proxy", sc_core::SC_ZERO_TIME, &mem);
	Terminal term("Terminal", clock);
	DeviceProxy term_proxy("term_proxy", sc_core::SC_ZERO_TIME, &term);
	Clint clint("Clint", clock, &core);
	DeviceProxy clint_proxy("clint_proxy", sc_core::SC_ZERO_TIME, &clint);
	ELFLoader loader(opt.input_program.c_str());
	CombinedMemoryInterface iss_mem_if("MemoryInterface", sc_core::SC_ZERO_TIME, max_txns);
	SyscallHandler sys;
	Bus<1,4> bus("bus");
	
//=========================================================================================
	Data_loader data_loader(opt.input_data.c_str());
	ReconfigController rc("rc");
	DeviceProxy rc_proxy("rc_proxy", sc_core::SC_ZERO_TIME, &rc);
	RFA rfa("rfa",&rc);

	Memory data_mem("data_memory", DATA_MEM_SIZE, clock_period, clock_period);	//32K字

	DeviceProxy deviceproxy0("deviceproxy0", sc_core::SC_ZERO_TIME, &data_mem);
	DeviceProxy deviceproxy1("deviceproxy1", sc_core::SC_ZERO_TIME, &data_mem);
	DeviceProxy deviceproxy2("deviceproxy2", sc_core::SC_ZERO_TIME, &data_mem);
	DeviceProxy deviceproxy3("deviceproxy3", sc_core::SC_ZERO_TIME, &data_mem);
	DeviceProxy deviceproxy4("deviceproxy4", sc_core::SC_ZERO_TIME, &data_mem);
	DeviceProxy deviceproxy5("deviceproxy5", sc_core::SC_ZERO_TIME, &data_mem);

	rfa.isock[0].bind(deviceproxy0.tsock);
	rfa.isock[1].bind(deviceproxy1.tsock);
	rfa.isock[2].bind(deviceproxy2.tsock);
	rfa.isock[3].bind(deviceproxy3.tsock);
	rfa.isock[4].bind(deviceproxy4.tsock);
	rfa.isock[5].bind(deviceproxy5.tsock);

	rc.m_rfa_ptr = &rfa;
//=========================================================================================
	
	bus.ports[0] = new PortMapping(opt.mem_start_addr, opt.mem_end_addr);
	bus.ports[1] = new PortMapping(opt.term_start_addr, opt.term_end_addr);
	bus.ports[2] = new PortMapping(opt.clint_start_addr, opt.clint_end_addr);
	bus.ports[3] = new PortMapping(opt.rfu_start_addr, opt.rfu_end_addr);
	
	loader.load_executable_image(mem.m_data, mem.m_size, opt.mem_start_addr);
	data_loader.load_data(data_mem.m_data, data_mem.m_size);
	core.init(&iss_mem_if, &iss_mem_if, &clint, &sys, loader.get_entrypoint(), opt.mem_end_addr - 4);
	sys.init(mem.m_data, opt.mem_start_addr, loader.get_heap_addr());
	
	iss_mem_if.isock.bind(bus.tsock[0]);
	bus.isock[0].bind(memory_proxy.tsock);
	bus.isock[1].bind(term_proxy.tsock);
	bus.isock[2].bind(clint_proxy.tsock);
	bus.isock[3].bind(rc_proxy.tsock);

	clint.m_target = &core;

	new DirectCoreRunner(core);

	sc_core::sc_start();

	core.show();

	data_loader.extract_data(data_mem.m_data, data_mem.m_size);

	return 0;

}

