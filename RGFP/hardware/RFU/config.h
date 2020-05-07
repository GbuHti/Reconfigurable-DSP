#ifndef CONFIG_H
#define CONFIG_H

#include "systemc.h"
#include "tlm.h"

#include "contextreg_if.h"

#define GENERATE_ARITH_PE_SLC(lgid,op,mux_a,mux_b) \
	(lgid<<27) + (op<<24) + (mux_a<<19) + (mux_b<<14) 

using namespace std;
using namespace sc_core;

class Config : public sc_module
{
	private:
		enum { BROADCAST_MAX = 64};
		unsigned m_broadcast_num;
	public:
		contextreg_if * config[BROADCAST_MAX];
		SC_HAS_PROCESS(Config);
		Config
		( sc_module_name name
		, unsigned broadcast_num
		)
		: sc_module(name)
		, m_broadcast_num(broadcast_num)
		{
			SC_THREAD(send_context);	
		}

		void send_context()
		{
			slc context;
			assert( m_broadcast_num <= BROADCAST_MAX && "Broadcast node exceed! ");
			for(unsigned i = 0; i<m_broadcast_num; i++)
			{
				context.reg = GENERATE_ARITH_PE_SLC(4, 0, 2 ,1);	
				config[i]->write_context_reg(context);
				context.reg = GENERATE_ARITH_PE_SLC(8, 0, 5 ,3);	
				config[i]->write_context_reg(context);
			}
			for(unsigned i = 0; i<m_broadcast_num; i++)
			{
				config[i]->all_config();
			}
		}
};

#endif

