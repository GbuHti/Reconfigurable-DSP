#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"

const sc_core::sc_time clock_period = sc_time(10,sc_core::SC_NS);

#define ARITH_PE_NUM	8
#define LOADER_NUM		4
#define STORER_NUM		2

#define NORMAL_COMPUTE	tlm::TLM_READ_COMMAND
#define LAST_COMPUTE	tlm::TLM_WRITE_COMMAND

#endif
