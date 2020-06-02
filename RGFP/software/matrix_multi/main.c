#include "stdio.h"
#include "stdint.h"

#define BIT_RANGE(instr,upper,lower) ((uint32_t)instr & (((1 << (upper-lower+1)) - 1) << lower))
#define BIT_SLICE(instr,upper,lower) (BIT_RANGE(instr,upper,lower) >> lower)

#define GENERATE_LOADER_FLC(lgid, op, batch_len, op_aux, ext_aux, addr_inc,addr) \
	(lgid<<28) + (op<<25) + (BIT_SLICE(batch_len,9,0)<<15) + (op_aux<<14) + (ext_aux<<13) + (BIT_SLICE(addr_inc,6,0)<<5) + BIT_SLICE(addr,16,12)

#define GENERATE_ARITH_PE_FLC(lgid, op, mux_a, mux_b, op_aux) \
	(lgid<<28) + (op<<25) + (mux_a<<20) + (mux_b<<15) + (op_aux<<14) 

#define GENERATE_STORER_FLC(lgid, op, mux_a, op_aux, ext_aux, addr_inc, addr) \
	(lgid<<28) + (op<<25) + (mux_a<<20) + (op_aux<<14) + (ext_aux << 13) + (BIT_SLICE(addr_inc,6,0)<<5) + BIT_SLICE(addr,16,12)

#define GENERATE_FLC_EXT(batch_len, addr_inc, addr) \
	(BIT_SLICE(batch_len,14,10)<<20) + (BIT_SLICE(addr_inc,11,7)<<15) + BIT_SLICE(addr,11,0)

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
#define MATRIX_SCALE 128 

void matrix_multi(float *, float *, float *, unsigned);

int main()
{
	float *a = (float *)0x0;
	float *b = a;
	float *c = (float *)(8*1024*4);	

	matrix_multi(a,b,c,MATRIX_SCALE);

	return 0;
}

void matrix_multi(float *ma, float *mb, float *mc, unsigned scale) //可封装成库
{
	for(unsigned i = 0; i < scale; i++)
	{
		for(unsigned j = 0; j < scale; j++)
		{
			uint32_t flc;
			float * addr = ma + i*scale; //利用指针的算术运算规则
			//dot product
			flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, scale, 0, 1, 1, addr); //随机存取模式 slc::ext_aux = 1
			*(uint32_t *)FLC_ADDR = flc;
			flc = GENERATE_FLC_EXT(0,0,addr);								  //TWO FLCs
			*(uint32_t *)FLC_ADDR = flc;
			addr = mb + j; //
			flc = GENERATE_LOADER_FLC(1,CONFIG_LOAD, scale, 0, 1, scale, addr);
			*(uint32_t *)FLC_ADDR = flc;
			flc = GENERATE_FLC_EXT(0,0,addr);								 
			*(uint32_t *)FLC_ADDR = flc;
			flc = GENERATE_ARITH_PE_FLC(2,CONFIG_MUL, 0, 1, 0);	
			*(uint32_t *)FLC_ADDR = flc;
			flc = GENERATE_ARITH_PE_FLC(3,CONFIG_ADD, 2, 3, 0);	
			*(uint32_t *)FLC_ADDR = flc;
			addr = mc + i*scale + j; //
			flc = GENERATE_STORER_FLC(4, CONFIG_STORE, 3, 0, 1, 0, addr);	
			*(uint32_t *)FLC_ADDR = flc;
			flc = GENERATE_FLC_EXT(0,0,addr);								 
			*(uint32_t *)FLC_ADDR = flc;
		}	
	}	
}

