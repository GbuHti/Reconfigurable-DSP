#include "ReconfigController.h"
#include "define.h"

#include "report.h"
using namespace std;

static const char *filename = "ReconfigController.cpp";  ///< filename for reporting

SC_HAS_PROCESS(ReconfigController);

ReconfigController::ReconfigController
( sc_module_name name
)
: sc_module(name)
, m_dp(0)
, m_status(IDLE)
{
	SC_THREAD(Preprocessing_thread);
	SC_THREAD(Allocate_thread);
	SC_THREAD(Dispatch_thread);
	SC_THREAD(Monitor_busy_thread);

	m_flc.reg = 0;

	m_slcs[0].reg = GENERATE_LOADER_SLC(0,CONFIG_LOAD,0,0,0,0,0);
	m_slcs[1].reg = GENERATE_LOADER_SLC(1,CONFIG_LOAD,0,0,0,0,0);
	m_slcs[2].reg = GENERATE_LOADER_SLC(2,CONFIG_LOAD,0,0,0,0,0);
	m_slcs[3].reg = GENERATE_LOADER_SLC(3,CONFIG_LOAD,0,0,0,0,0);

	m_slcs[4].reg = GENERATE_ARITH_PE_SLC(4,CONFIG_ADD,0,0,0,0);
	m_slcs[5].reg = GENERATE_ARITH_PE_SLC(5,CONFIG_ADD,0,0,0,0);
	m_slcs[6].reg = GENERATE_ARITH_PE_SLC(6,CONFIG_ADD,0,0,0,0);
	m_slcs[7].reg = GENERATE_ARITH_PE_SLC(7,CONFIG_ADD,0,0,0,0);
	m_slcs[8].reg = GENERATE_ARITH_PE_SLC(8,CONFIG_MUL,0,0,0,0);
	m_slcs[9].reg = GENERATE_ARITH_PE_SLC(9,CONFIG_MUL,0,0,0,0);
	m_slcs[10].reg = GENERATE_ARITH_PE_SLC(10,CONFIG_SQRT,0,0,0,0);
	m_slcs[11].reg = GENERATE_ARITH_PE_SLC(11,CONFIG_SIN,0,0,0,0);

	m_slcs[12].reg = GENERATE_STORER_SLC(12,CONFIG_STORE,0,0,0,0,0);
	m_slcs[13].reg = GENERATE_STORER_SLC(13,CONFIG_STORE,0,0,0,0,0);

}

sc_core::sc_time ReconfigController::get_device_delay
(
 tlm::tlm_generic_payload &trans
 ,sc_core::sc_time &delay	
 )
{
	return delay;
}


void ReconfigController::operation // FSM
(
 tlm::tlm_generic_payload &trans
 , sc_core::sc_time &delay 
)
//{{{
{
	assert( trans.get_command() == tlm::TLM_WRITE_COMMAND);
	unsigned char * data_ptr = trans.get_data_ptr();	

	m_flc_next.reg = *(uint32_t *)data_ptr; 

	switch(m_status) //在verilog FSM中，FSM的ns由当前周期内的输入决定，在下一个时钟沿到来时，
	{				 //打入CS中，此时，输入因保持时间仍保持不变，可以在CS状态下被采样处理
		case IDLE:   //总的来看就是，CS更新状态-->根据CS状态在时钟沿采样输入，的过程。
			if(m_flc_next.ext_aux){ //在systemc中模拟的就是这一过程。
				m_status = INDIRECT_MODE_0;	
			}else{
				m_status = DIRECT_MODE;	
			}	
			break;	
		case DIRECT_MODE:
			if(m_flc_next.ext_aux){
				m_status = INDIRECT_MODE_0;	
			}else{
				m_status = DIRECT_MODE;		
			}
			break;
		case INDIRECT_MODE_0:
			m_status = INDIRECT_MODE_1;
			break;
		case INDIRECT_MODE_1:
			if(m_flc_next.ext_aux){
				m_status = INDIRECT_MODE_0;	
			}else {
				m_status = DIRECT_MODE;	
			}
			break;
		default:
			assert(false && "Unsupported STATUS in RC");
			break;
	}
	trigger_evnet.notify(SC_ZERO_TIME);
}


void ReconfigController::Preprocessing_thread()
{

	std::ostringstream  msg;                      ///< log message

	while(true)
	{
		wait(trigger_evnet);
		//相关性检测
		if(m_status == DIRECT_MODE || m_status == INDIRECT_MODE_0)
		{
			if( m_flc_next.is_load() && m_flc.is_store())
			{
				if(m_flc_next.addr == m_flc.addr)
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
			m_flc = m_flc_next;
			if(m_status == DIRECT_MODE)
			{
				begin_allocate_event.notify(sc_core::SC_ZERO_TIME);	
				wait(finish_allocate_event); //进程阻塞，是否要改进？
			}
		}
		else if(m_status == INDIRECT_MODE_1)
		{
			m_flc_ext.reg = m_flc_next.reg;	
			begin_allocate_event.notify(sc_core::SC_ZERO_TIME);	
			wait(finish_allocate_event);
		}

	}
}
//}}}

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
							it->second.addr			= (m_flc.addr<<12) + m_flc_ext.addr_tail;
							it->second.addr_inc		= (m_flc_ext.addr_inc_head << 7) + m_flc.addr_inc;
							it->second.op_aux		= m_flc.op_aux;		
							it->second.batch_len	= (m_flc_ext.batch_len_head << 10) + m_flc.batch_len;
							break;
						case CONFIG_ADD:
						case CONFIG_MUL:
						case CONFIG_SIN:
						case CONFIG_SQRT:
							it->second.op_aux		= m_flc.op_aux;
							it->second.mux_b		= m_translation_table[m_flc.mux_b];
							it->second.mux_a		= m_translation_table[m_flc.mux_a];
							break;
						case CONFIG_STORE:
							it->second.addr			= (m_flc.addr << 12) + m_flc_ext.addr_tail;
							it->second.addr_inc		= (m_flc_ext.addr_inc_head << 7) + m_flc.addr_inc;
							it->second.mux_a		= m_translation_table[m_flc.mux_a];
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
//=================================================================================
void ReconfigController::Dispatch_thread()
{
	while(true)
	{
		wait(dispatch_event);
		for(unsigned i = 0; i < sizeof(uint32_t)*8; i++)
		{
			if( m_dp & (1<<i) )	
			{
				assert( m_rfa_ptr != 0 && "You should connect RC with RFA");
				m_rfa_ptr->write_context_reg(m_slcs[i]);
				if(isStore(m_slcs[i]))
				{
					m_rfa_ptr->all_config();	
				}
				m_dp = m_dp & ~(1<<i); //派遣成功后，将对应的位置零；
			}
		}	
	}
}
//=================================================================================
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

//=================================================================================
void ReconfigController::release_busy(unsigned id)
{
	m_slcs[id].busy = 0;	
	busy_changed_event.notify(sc_core::SC_ZERO_TIME);
}





