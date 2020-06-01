#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"
#include "instr.h" // 将它的比特域选择宏定义包括进来

const sc_core::sc_time clock_period = sc_time(10,sc_core::SC_NS);

#define ARITH_PE_NUM	8
#define LOADER_NUM		4
#define STORER_NUM		2

#define NORMAL_COMPUTE	tlm::TLM_READ_COMMAND
#define LAST_COMPUTE	tlm::TLM_WRITE_COMMAND

#define DATA_MEM_SIZE 1024*4*32 //32K字
#endif
