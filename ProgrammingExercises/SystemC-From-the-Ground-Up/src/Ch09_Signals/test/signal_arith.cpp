#include <systemc>
#include <cmath>
#include "signal_arith.h"
using namespace std;
using namespace sc_core;

void Signal_arith::arith_thread(){
	r = x;
	if(r != 0 && r != 1) r = r*r;
	if(y != 0) r = r + y*y;
	cout << "Radius is " << r  << endl;
	wait(SC_ZERO_TIME);
	cout << "Radius is " << r  << endl;
}

void Signal_arith::competi_thread(){
	r = x*x;
	cout << "Radius is " << r << endl;
	wait(SC_ZERO_TIME);
	cout << "Radius is " << r << endl;
}
