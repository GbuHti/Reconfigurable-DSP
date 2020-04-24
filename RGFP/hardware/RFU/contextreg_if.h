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

#define GENERATE_PE_SLC(a,b,c,d,e,f) \
	(a<<27) + (b<<24) + (c<<19) + (d<<14) + (e<<13) + (f<<12)
#define GENERATE_LOADER_SLC(a,b,c,d,e,f,g) \
	(a<<27) + (b<<24) + (c<<14) + (d<<13) + (e<<12) + (f<<5) + g

struct slc
{
	union
	{
		uint32_t	reg;
		struct
		{
			unsigned reserved	:12;
			unsigned busy		:1;
			unsigned op_aux		:1;
			unsigned mux_b		:5;
			unsigned mux_a		:5;
		};
		struct
		{
			unsigned addr		:5;
			unsigned addr_inc	:7;
			unsigned knalb		:2;
			unsigned batch_len	:10;	
		};

		struct
		{
			unsigned blank		:24;
			unsigned op			:3;
			unsigned phid		:5;
		};
	};

	//Constuctor
};



class contextreg_if
{
	public:
		virtual void write_context_reg( slc context) = 0;
};

#endif
