#include "reporting.h"
#include "monitor.h"

using namespace std;
using namespace sc_core;

static const char *filename = "monitor.cpp";

SC_HAS_PROCESS(monitor);

monitor::monitor(sc_core::sc_module_name name)
	: sc_module(name)
	, monitor_isocket("monitor_isocket")
	{
		SC_THREAD(monitor_thread);	
	}

void monitor::monitor_thread(void)
{
	wait(100,SC_NS);
	std::ostringstream msg;

	tlm::tlm_generic_payload trans;
	int data_buffer;
	sc_dt::uint64 address = 0x0000000000000004;

	trans.set_command(tlm::TLM_READ_COMMAND);
	trans.set_address(address);
	trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data_buffer));
	trans.set_data_length(4);
	trans.set_streaming_width(4);
	trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

	sc_time delay = SC_ZERO_TIME;

	msg.str("");
	msg << "Monitor------> Starting\n";
	msg << "b_transport(GP, "
		<< delay << ")";
	REPORT_INFO(filename, __FUNCTION__, msg.str());

	monitor_isocket->b_transport(trans, delay);

	gp_status = trans.get_response_status();

	if(gp_status == tlm::TLM_OK_RESPONSE)
	{
		msg.str("");
		msg << "Monitor======> Ending\n";
		msg << "b_transport returned delay = "
			<< delay << "\n";
		msg << "Read data = " << data_buffer;
		REPORT_INFO(filename, __FUNCTION__, msg.str());
	}
	wait(20,SC_NS);
	if(gp_status == tlm::TLM_OK_RESPONSE)
	{
		msg.str("");
		msg << "Delaying........";
		msg << "Read data = " << data_buffer;
		REPORT_INFO(filename, __FUNCTION__, msg.str());
	}
}
