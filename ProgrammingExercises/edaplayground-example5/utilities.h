#ifndef UTILITIES_H
#define UTILITIES_H

#include <systemc>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"

//	User-defined memory manager, which maintains a pool of transcations

class mm: public tlm::tlm_mm_interface
{
	typedef tlm::tlm_generic_payload gp_t;
	 
	public:
	mm():free_list(0),empties(0){}

	gp_t * allocate();
	void free(gp_t *trans);

};
#endif
