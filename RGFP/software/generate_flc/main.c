#include "stdio.h"

#define GENERATE_PE_FLC(lgid,op,mux_a,mux_b,op_aux) \
	(lgid<<28) + (op<<25) + (mux_a<<20) + (mux_b<<15) + (op_aux<<14) 
#define GENERATE_LOADER_FLC(lgid,op,batch_len,op_aux,addr_inc,addr) \
	(lgid<<28) + (op<<25) + (batch_len<<15) + (op_aux<<14) + (addr_inc<<5) + addr

enum op
{
	CONFIG_ADD		= 0b000,
	CONFIG_MUL		= 0b001,
	CONFIG_SQRT		= 0b010,
	CONFIG_SIN		= 0b011,
	CONFIG_LOAD		= 0b100,
	CONFIG_STORE	= 0b101,
};

#define	FLC_ADDR  0x30000000
int main()
{
	uint32_t flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, 512, 0, 1, 0);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_LOADER_FLC(1,CONFIG_LOAD, 512, 0, 1, 0);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_PE_FLC(2,CONFIG_ADD, 0, 1, 0);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_PE_FLC(3,CONFIG_ADD, 0, 1, 1);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_PE_FLC(4,CONFIG_ADD, 0, 1, 0);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_PE_FLC(5,CONFIG_ADD, 0, 1, 0);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_PE_FLC(6,CONFIG_ADD, 0, 1, 0);	
	*(uint32_t *)FLC_ADDR = flc;
	
	return 0;
}
