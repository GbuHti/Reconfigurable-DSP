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
			unsigned reserved	:14;
			unsigned op_aux		:1;
			unsigned mux_b		:5;
			unsigned mux_a		:5;
		};

		struct
		{
			unsigned addr		:5;				
			unsigned addr_inc	:7;
			unsigned devrdser	:2;
			unsigned knalb		:1;
			unsigned batch_len	:10;
		};
		
		struct
		{
			unsigned blank		:25;
			unsigned op			:3;
			unsigned lgid		:4;
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


#define GENERATE_LOADER_SLC(lgid, op, batch_len, aux, busy, addr_inc, addr) \
	(lgid<<27) + (op<<24) + (batch_len<<14) + (aux<<13) + (busy<<12) + (addr_inc << 5) + (addr) 
#define GENERATE_ARITH_PE_SLC(lgid, op, mux_a, mux_b, aux, busy) \
	(lgid<<27) + (op<<24) + (mux_a<<19) + (mux_b<<14) + (aux << 13) + (busy << 12)
#define GENERATE_STORER_SLC(lgid, op, mux_a, aux, busy, addr_inc, addr) \
	(lgid<<27) + (op<<24) + (mux_a<<19) + (aux<<13) + (busy<<12) + (addr_inc<<5) + (addr) 


struct slc
{
	union
	{
		uint32_t	reg;
		struct
		{
			unsigned reserved	   :12;
			unsigned busy	 	   :1;
			unsigned op_aux	 	   :1;
			unsigned mux_b	 	   :5;
			unsigned mux_a	 	   :5;
		};
		struct
		{
			unsigned addr			:5;
			unsigned addr_inc		:7;
			unsigned placeholder	:2;
			unsigned batch_len		:10;	
		};

		struct
		{
			unsigned blank		:24;
			unsigned op			:3;
			unsigned phid		:5;
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
