#ifndef SIGNAL_ARITH_H
#define SIGNAL_ARITH_H

#include <systemc>
using namespace sc_core;

SC_MODULE(Signal_arith){
	sc_signal<int> r;
	int x;
	int y;

	SC_CTOR(Signal_arith)
	:x(3),y(4){
		SC_THREAD(arith_thread);
		SC_THREAD(competi_thread);
	}

	void arith_thread();
	void competi_thread();
};
#endif
