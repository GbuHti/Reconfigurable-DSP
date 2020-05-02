#ifndef BASIC_PROCESS_EX_H
#define BASIC_PROCESS_EX_H
#include "systemc.h"

SC_MODULE(basic_process_ex){
//	SC_CTOR(basic_process_ex){
//		SC_THREAD(my_thread_process);
//	}	
	basic_process_ex (sc_module_name instname);
	void my_thread_process(void);
	void wcx_thread_process(void);
};

#endif
