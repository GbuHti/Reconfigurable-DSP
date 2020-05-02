#include <systemc>
#include "basic_process_ex.h"
using namespace sc_core;

int sc_main(int argc, char* argv[]) {
	basic_process_ex my_instance("my_instance");
	basic_process_ex wcx_instance("wcx_thread_process");
	cout << "The end of elaboration and begining of simulation" << endl;
	sc_start();
	return 0;
}
