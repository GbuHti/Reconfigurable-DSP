#include "stdio.h"
#include "stdint.h"

#define BIT_RANGE(instr,upper,lower) (instr & (((1 << (upper-lower+1)) - 1) << lower))
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
int main()
{
	uint32_t flc;
	//set 1
//	flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, 32, 0, 1, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(1,CONFIG_ADD, 0, 1, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_STORER_FLC(2, CONFIG_STORE, 1, 0, 0, 1);	
//	*(uint32_t *)FLC_ADDR = flc;
//
//	//set 2
//	flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, 32, 0, 1, 0);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_LOADER_FLC(1,CONFIG_LOAD, 32, 0, 1, 0);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(2,CONFIG_ADD, 0, 1, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(3,CONFIG_ADD, 0, 2, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(4,CONFIG_SQRT, 3, 0, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_STORER_FLC(5, CONFIG_STORE, 4, 0, 1, 2);	
//	*(uint32_t *)FLC_ADDR = flc;

	//dot product
	flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, 128, 0, 1, 1, 512*4);
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_FLC_EXT(0,0,512*4);
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_LOADER_FLC(1,CONFIG_LOAD, 128, 0, 1, 1, 256*4);
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_FLC_EXT(0,0,256*4);
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_ARITH_PE_FLC(2,CONFIG_MUL, 0, 1, 0);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_ARITH_PE_FLC(3,CONFIG_ADD, 2, 3, 0);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_STORER_FLC(4, CONFIG_STORE, 3, 0, 1, 0, 8*1024+4);	
	*(uint32_t *)FLC_ADDR = flc;
	flc = GENERATE_FLC_EXT(0,0,8*1024+4);
	*(uint32_t *)FLC_ADDR = flc;
	
	//dot product (非连续)
//	flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, 512, 0, 1, 0);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_LOADER_FLC(1,CONFIG_LOAD, 512, 0, 1, 0);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(2,CONFIG_MUL, 0, 1, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_STORER_FLC(3, CONFIG_STORE, 2, 0, 1, 8);	
//	*(uint32_t *)FLC_ADDR = flc;
//
//	flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, 512, 0, 1, 8);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(1,CONFIG_ADD, 0, 1, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_STORER_FLC(2, CONFIG_STORE, 1, 0, 0, 8);	
//	*(uint32_t *)FLC_ADDR = flc;

//const int N = 96;	
//for(int i = 0; i < N; i++)
//{
//	for(int j = 0; j < N; j++)
//	{
//		flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, N, 0, 1, 0);
//		*(uint32_t *)FLC_ADDR = flc;
//		flc = GENERATE_LOADER_FLC(1,CONFIG_LOAD, N, 0, N, 0);
//		*(uint32_t *)FLC_ADDR = flc;
//		flc = GENERATE_ARITH_PE_FLC(2,CONFIG_MUL, 0, 1, 0);	
//		*(uint32_t *)FLC_ADDR = flc;
//		flc = GENERATE_ARITH_PE_FLC(3,CONFIG_ADD, 2, 3, 0);	
//		*(uint32_t *)FLC_ADDR = flc;
//		flc = GENERATE_STORER_FLC(4, CONFIG_STORE, 3, 0, 0, 0);	//store 地址设为和store一样，串行执行
//		*(uint32_t *)FLC_ADDR = flc;
//	}	
//	
//}

// 一般计算模式，但是这种还是会并行执行
//const int N = 64;	
//for(int i = 0; i < N; i++)
//{
//	for(int j = 0; j < N; j++)
//	{
//	flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, N, 0, 1, 0);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_LOADER_FLC(1,CONFIG_LOAD, N, 0, 1, 0);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(2,CONFIG_MUL, 0, 1, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_STORER_FLC(3, CONFIG_STORE, 2, 0, 1, 8);	
//	*(uint32_t *)FLC_ADDR = flc;
//
//	flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, N, 0, 1, 8);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(1,CONFIG_ADD, 0, 1, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_STORER_FLC(2, CONFIG_STORE, 1, 0, 0, 8);	
//	*(uint32_t *)FLC_ADDR = flc;
//	}	
//	
//}
//	//向量加
//	flc = GENERATE_LOADER_FLC(0,CONFIG_LOAD, 512, 0, 1, 0);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_LOADER_FLC(1,CONFIG_LOAD, 512, 0, 1, 0);
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_ARITH_PE_FLC(2,CONFIG_ADD, 0, 1, 0);	
//	*(uint32_t *)FLC_ADDR = flc;
//	flc = GENERATE_STORER_FLC(4, CONFIG_STORE, 2, 0, 1, 1);	
//	*(uint32_t *)FLC_ADDR = flc;


	//set 3

	return 0;
}
