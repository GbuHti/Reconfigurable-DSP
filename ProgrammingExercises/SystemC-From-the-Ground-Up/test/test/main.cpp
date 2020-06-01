#include "systemc.h"

using namespace std;
using namespace sc_core;

class Module_A:public sc_module
{
	private:
		bool m_data_used;
		sc_event m_data_used_event;
	public:
		SC_HAS_PROCESS(Module_A);

		Module_A(sc_module_name name)
			:sc_module(name)
			,m_data_used(false)
		{

			SC_THREAD(proc_A);
			SC_THREAD(proc_B);	
		}

		void proc_A()
		{
			wait(10,SC_NS);	
			m_data_used = true;
			m_data_used_event.notify(SC_ZERO_TIME);
		}

		void proc_B()
		{
			while(true)
			{
//				wait(m_data_used_event);
				if(m_data_used) //如果不在循环中加入用于advance time的语句，则永远卡在这里，控制权无法收回
					cout << "catch it!!!" << endl;	
			}	
		}
};

int sc_main(int argc, char *argv[])
{
	Module_A test("test");
	sc_start();

	return 0;	
}
