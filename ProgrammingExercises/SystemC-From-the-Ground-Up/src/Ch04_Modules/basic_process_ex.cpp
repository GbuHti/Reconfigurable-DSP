#include <systemc>
#include "basic_process_ex.h"
using namespace std;
SC_HAS_PROCESS(basic_process_ex);
basic_process_ex::basic_process_ex(
	sc_module_name instname)
	:sc_module(instname){
		SC_THREAD(my_thread_process);	
		SC_THREAD(wcx_thread_process);	
	}

void basic_process_ex::my_thread_process(void){
	cout << "my_thread_process executed within "
		<< name()
		<< endl;
}

void basic_process_ex::wcx_thread_process(void){
	cout << "wcx_thread_process executed within "
		<< name()
		<< endl;
}
