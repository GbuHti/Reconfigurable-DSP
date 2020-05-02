#include "systemc.h"

SC_MODULE(helloworld){

	sc_event m_event;
	void print_hello(){
		cout << "Hello World! SystemC!" << endl;
		m_event.notify(10,sc_core::SC_NS);
	}

	void proc_thread(){
		for(int i = 0; i<4; i++)
		{
			std::cout << i << ": " << sc_core::sc_time_stamp() << std::endl;
			std::cout << i << ": "<< sc_core::sc_time_stamp().value() << std::endl;
			std::cout << i << ": "<< sc_core::sc_time_stamp().to_double() << std::endl;
			std::cout << i << ": "<< sc_core::sc_time_stamp().to_seconds() << std::endl;
		}	
		next_trigger(20,sc_core::SC_NS);
		std::cout << "=====================" << std::endl;
	}

	SC_HAS_PROCESS(helloworld);
	helloworld(sc_core::sc_module_name ){
//	SC_CTOR(helloworld){
		SC_THREAD(print_hello);
		SC_METHOD(proc_thread);
			sensitive << m_event;
			dont_initialize();	
	}
};

int sc_main(int argc, char *argv[]){	

	helloworld hello("hello");
	sc_start();

	return 0;
}
