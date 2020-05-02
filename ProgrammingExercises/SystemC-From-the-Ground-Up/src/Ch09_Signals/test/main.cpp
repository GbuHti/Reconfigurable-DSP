#include <systemc>
#include "signal_arith.h"

int sc_main(int argc, char * argv[]){
	Signal_arith Signal_arith_i("Signal_arith_i");
	sc_core::sc_start();	
	return 0;
}
