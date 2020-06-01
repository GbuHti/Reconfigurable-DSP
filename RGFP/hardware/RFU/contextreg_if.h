#ifndef CONTEXTREG_IF_H
#define CONTEXTREG_IF_H

#include "systemc.h"


enum op
{
	CONFIG_ADD		= 0b000,
	CONFIG_MUL		= 0b001,
	CONFIG_SQRT		= 0b010,
	CONFIG_SIN		= 0b011,
	CONFIG_LOAD		= 0b100,
	CONFIG_STORE	= 0b101,
};

struct flc
{
	union
	{
		uint32_t reg;
		struct  
		{
			unsigned reserved	:13;
			unsigned ext_aux	:1; // 指示下一个数据的解释方法：0:flc；1: flc_ext
			unsigned op_aux		:1;
			unsigned mux_b		:5;
			unsigned mux_a		:5;
		};

		struct
		{
			unsigned addr			:5;				
			unsigned addr_inc		:7;
			unsigned placeholder_0	:2;
			unsigned placeholder_1	:1;
			unsigned batch_len		:10;
		};
		
		struct
		{
			unsigned placeholder_3	:25;
			unsigned op				:3;
			unsigned lgid			:4;
		};
	};	

	//Constructor

	inline bool is_load()
	{
		return op == CONFIG_LOAD;	
	}

	inline bool is_store()
	{
		return op == CONFIG_STORE;	
	}
	
};

struct flc_ext
{
	union
	{
		uint32_t reg;
		struct
		{
			unsigned addr_tail		:12; //地址地位部分 组合方式：{flc.addr, flc_ext.addr_tail}
			unsigned reserved_0		:3;
			unsigned addr_inc_head	:5;
			unsigned batch_len_head	:5;	
			unsigned reserved_1		:7;
		};	
	};
};

#define GENERATE_LOADER_SLC(lgid, op, op_aux, busy, batch_len, addr_inc, addr) \
	(uint64_t(lgid) << 59) + (uint64_t(op) << 56) + (uint64_t(op_aux) << 55) + (uint64_t(busy) << 54) + (uint64_t(batch_len) << 39) + (uint64_t(addr_inc) << 17) + (addr)

#define GENERATE_ARITH_PE_SLC(lgid, op, op_aux, busy, mux_a, mux_b) \
	(uint64_t(lgid) << 59) + (uint64_t(op) << 56) + (uint64_t(op_aux) << 55) + (uint64_t(busy) << 54) + (uint64_t(mux_a) << 49) + (uint64_t(mux_b) << 44)

#define GENERATE_STORER_SLC(lgid, op, op_aux, busy, mux_a, addr_inc, addr) \
	(uint64_t(lgid) << 59) + (uint64_t(op) << 56) + (uint64_t(op_aux) << 55) + (uint64_t(busy) << 54) + (uint64_t(mux_a) << 49) + (uint64_t(addr_inc) << 17) + (addr)

struct slc
{
	union
	{
		uint64_t reg;			//在RFU内部，位宽不必局限于32bit
		struct
		{
			uint64_t		reserved	   :44;
			uint64_t		mux_b	 	   :5;
			uint64_t		mux_a	 	   :5;
			uint64_t		busy	 	   :1;
			uint64_t		op_aux	 	   :1;
		};
		struct
		{
			uint64_t		addr			:17;
			uint64_t		addr_inc		:12;
			uint64_t		placeholder		:10;
			uint64_t		batch_len		:15;	
		};
		struct
		{
			uint64_t		blank		:56;
			uint64_t		op			:3;
			uint64_t		phid		:5;
		};
	};

	//Helper function
	bool isArith()
	{return op == CONFIG_ADD | op == CONFIG_MUL | op == CONFIG_SQRT | op == CONFIG_SIN;}

	bool isStore()
	{return op == CONFIG_STORE;}

};



class contextreg_if
{
	public:
		virtual void write_context_reg( slc context) = 0;
		virtual void all_config() = 0;
};

#endif
