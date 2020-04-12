#include "systemc.h"

SC_MODULE(helloworld){
	void print_hello(){
		cout << "Hello World! SystemC!" << endl;
	}

	void proc_thread(){
		for(int i = 0; i<4; i++)
		{
			wait(10,SC_NS);
			std::cout << i << ": " << sc_core::sc_time_stamp() << std::endl;
			std::cout << i << ": "<< sc_core::sc_time_stamp().value() << std::endl;
			std::cout << i << ": "<< sc_core::sc_time_stamp().to_double() << std::endl;
			std::cout << i << ": "<< sc_core::sc_time_stamp().to_seconds() << std::endl;
		}	
	}

	SC_HAS_PROCESS(helloworld);
	helloworld(sc_core::sc_module_name ){
//	SC_CTOR(helloworld){
		SC_METHOD(print_hello);
		SC_THREAD(proc_thread);
	}
};

int sc_main(int argc, char *argv[]){	

	helloworld hello("hello");
	sc_start();

	return 0;
}
