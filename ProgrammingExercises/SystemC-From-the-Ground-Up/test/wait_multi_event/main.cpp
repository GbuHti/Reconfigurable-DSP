#include "systemc.h"
using namespace sc_core;
using namespace std;

class SimpleModule:public sc_module
{
	private:
		sc_event ready[2];
		int cnt = 0;

	public:
	SC_HAS_PROCESS(SimpleModule);

	SimpleModule(sc_module_name name)
		:sc_module	(name)
	{
		SC_THREAD(portA);
		SC_THREAD(portB);
		SC_THREAD(portC);
		SC_THREAD(proc_thread);
	}

	void portA()
	{
		ready[0].notify(10,SC_NS);
		wait(ready[0]);
		cout << "portA trigger event at " << sc_time_stamp() << endl;
		while(true)
		{
			cnt++;
			wait(6, sc_core::SC_NS);	
		}
	}

	void portB()
	{
		ready[1].notify(20,SC_NS);
		wait(ready[1]);
		cout << "portB trigger event at " << sc_time_stamp() << endl;
	}

	void portC()
	{
		wait(ready[0] | ready[1]);
		cout << "portC start at " << sc_time_stamp() << endl;
	}

	void proc_thread()
	{
		wait(ready[0]);
		while(1)
		{
			cout << "TIME: " << sc_time_stamp() << endl;
			for (int i = 0; i<10; i++)
			{
				if(i + cnt == 11)
				{
					cout << "I got i == 11!!" << endl;
					return;
					break;
				}	
				cout << "i = " << i << endl; //上面的break会导致跳出for循环，i输出到2为止
			}	
			cnt++;
			wait(10,sc_core::SC_NS); //在循环中，wait的节拍是恒定的，在未来的动作发生的时间点是确定的。
			
		}	
	}
};

int sc_main(int argc, char * argv[]){
	
	SimpleModule test_obj("test_obj");
	sc_start(100, sc_core::SC_NS);	
	return 0;
}
