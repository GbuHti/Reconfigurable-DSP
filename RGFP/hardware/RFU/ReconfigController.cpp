#include "ReconfigController.h"
#include "define.h"

#include "report.h"
using namespace std;

static const char *filename = "ReconfigController.cpp";  ///< filename for reporting

SC_HAS_PROCESS(ReconfigController);

ReconfigController::ReconfigController
(
 sc_module_name name
 )
:sc_module(name)
,m_dp(0)
{
	SC_THREAD(Allocate_thread);
	SC_THREAD(Dispatch_thread);
	SC_THREAD(Monitor_busy_thread);

	m_flc.reg = 0;

	m_slcs[0].reg = GENERATE_LOADER_SLC(0,CONFIG_LOAD,0,0,0,0,0);
	m_slcs[1].reg = GENERATE_LOADER_SLC(1,CONFIG_LOAD,0,0,0,0,0);
	m_slcs[2].reg = GENERATE_LOADER_SLC(2,CONFIG_LOAD,0,0,0,0,0);
	m_slcs[3].reg = GENERATE_LOADER_SLC(3,CONFIG_LOAD,0,0,0,0,0);

	m_slcs[4].reg = GENERATE_PE_SLC(4,CONFIG_ADD,0,0,0,0);
	m_slcs[5].reg = GENERATE_PE_SLC(5,CONFIG_ADD,0,0,0,0);
	m_slcs[6].reg = GENERATE_PE_SLC(6,CONFIG_ADD,0,0,0,0);
	m_slcs[7].reg = GENERATE_PE_SLC(7,CONFIG_MUL,0,0,0,0);
	m_slcs[8].reg = GENERATE_PE_SLC(8,CONFIG_MUL,0,0,0,0);
	m_slcs[9].reg = GENERATE_PE_SLC(9,CONFIG_SQRT,0,0,0,0);
	m_slcs[10].reg = GENERATE_PE_SLC(10,CONFIG_SIN,0,0,0,0);
	m_slcs[11].reg = GENERATE_PE_SLC(11,CONFIG_ADD,0,0,0,0);

	m_slcs[12].reg = GENERATE_PE_SLC(12,CONFIG_STORE,0,0,0,0);
	m_slcs[13].reg = GENERATE_PE_SLC(13,CONFIG_STORE,0,0,0,0);

}

sc_core::sc_time ReconfigController::get_device_delay
(
 tlm::tlm_generic_payload &trans
 ,sc_core::sc_time &delay	
 )
{
	return delay;
}


void ReconfigController::operation
(
 tlm::tlm_generic_payload &trans
 , sc_core::sc_time &delay 
)
//{{{
{
	std::ostringstream  msg;                      ///< log message
	assert( trans.get_command() == tlm::TLM_WRITE_COMMAND);

	unsigned char * data_ptr = trans.get_data_ptr();	

	flc flc_next;
	flc_next.reg = *(uint32_t *)data_ptr; 

	//相关性检测
	if( flc_next.is_load() && m_flc.is_store())
	{
		if(flc_next.addr == m_flc.addr)
		{
			m_raw_slcs_num = m_translation_table[m_flc.lgid]; //使用flc的lgid在转换表中，找到phid

			if(m_slcs[m_raw_slcs_num].busy )
			{
				begin_monitor_busy_event.notify(clock_period);

				msg.str ("");
				msg << "RAW detected at: " << sc_time_stamp();
				REPORT_INFO(filename, __FUNCTION__, msg.str());

				wait(end_monitor_busy_event);//阻塞flc的下发，直到相关(RAW)解除；保守的方案;
				//TODO: WAW相关检测	
			}

		}
	}

	//进入分配阶段
	m_flc = flc_next;
	begin_allocate_event.notify(sc_core::SC_ZERO_TIME);	

	wait(finish_allocate_event);
	
	delay += sc_core::SC_ZERO_TIME;
}
//}}}

void ReconfigController::Monitor_busy_thread()
{
	while(true)
	{
		wait(begin_monitor_busy_event);
		while(true)
		{
			wait(busy_changed_event);
			if(!m_slcs[m_raw_slcs_num].busy)
			{
				end_monitor_busy_event.notify(sc_core::SC_ZERO_TIME);
				break;
			}	
		}
			
	}
}

/**
 * @brief:
 * @note: 一直找，直到找到为止，此时处理器会被阻塞
 * TODO：改进为中断，释放处理器
 */

void ReconfigController::Allocate_thread()
{
	std::ostringstream  msg;                      ///< log message
	while(true)
	{
		wait(begin_allocate_event);
		std::map<unsigned,slc>::iterator it;
		bool has_found = false;
		while(!has_found)
		{
			for( it = m_slcs.begin(); it != m_slcs.end(); it++)
			{
				if( (it->second.op == m_flc.op) && (!it->second.busy) )	
				{
					msg.str ("");
					msg << "Allocated PE number: " << it->first << " @ " << sc_time_stamp();
					REPORT_INFO(filename, __FUNCTION__, msg.str());


					finish_allocate_event.notify(sc_core::SC_ZERO_TIME);
					dispatch_event.notify(sc_core::SC_ZERO_TIME);
					has_found = true;
					m_dp = m_dp | (1 << it->first);
					m_translation_table[m_flc.lgid] = it->first;	
					it->second.busy = 1;
					switch(m_flc.op)
					{
						case CONFIG_LOAD:
							it->second.addr = m_flc.addr;
							it->second.addr_inc = m_flc.addr_inc;
							it->second.op_aux = m_flc.op_aux;		
							it->second.batch_len = m_flc.batch_len;
							break;
						case CONFIG_ADD:
						case CONFIG_MUL:
						case CONFIG_SIN:
						case CONFIG_SQRT:
						case CONFIG_STORE:
							it->second.op_aux = m_flc.op_aux;
							it->second.mux_b = m_translation_table[m_flc.mux_b];
							it->second.mux_a = m_translation_table[m_flc.mux_a];
							break;
						default:
							assert(false && "ERROR: Unsupported operation type");
					}
					break;
				}
			}
			
			if(!has_found)
			{
				wait(clock_period);
				msg.str ("");
				msg << "Not found idle PE, loop again @ " << sc_time_stamp();
				REPORT_INFO(filename, __FUNCTION__, msg.str());
			}
		}		
	}
}

void ReconfigController::Dispatch_thread()
{
	while(true)
	{
		wait(dispatch_event);
		for(unsigned i = 0; i < sizeof(uint32_t)*8; i++)
		{
			if( m_dp & (1<<i) )	
			{
				m_trans.set_address(m_slcs[i].phid);
				m_trans.set_data_ptr((unsigned char *)&(m_slcs[i].reg));
				m_trans.set_command(tlm::TLM_WRITE_COMMAND);
				m_trans.set_data_length(sizeof(uint32_t));

				tlm::tlm_phase phase = tlm::BEGIN_REQ;
				sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
				tlm::tlm_sync_enum return_value;
				return_value = isock->nb_transport_fw(m_trans, phase, delay);
				
				assert(return_value==tlm::TLM_COMPLETED && "ERROR: only 1 phase transcations supported");
				m_dp = m_dp & ~(1<<i); //派遣成功后，将对应的位置零；
			}
		}	
	}
}







