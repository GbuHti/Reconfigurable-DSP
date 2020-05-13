#include "loader.h"
#include <cmath>
#include <iomanip>

using namespace std;

SC_HAS_PROCESS(Loader);

Loader::Loader
( sc_module_name name
, unsigned id
,sc_core::sc_time end_rsp_delay
)
: sc_module (name)
, m_ID(id)
, m_end_rsp_delay(end_rsp_delay)
, m_data_cnt(0)
, m_ready(false)
, m_ini_done(false)
, m_pool_size(2)
, m_addr_counter(0)
, m_advance_computing(0)
, mResponsePEQ("mResponsePEQ")
, mIssuePEQ("mIssuePEQ")
{
	isock_memory.register_nb_transport_bw(this, &Loader::nb_transport_bw_memory);
	isock_crossbar.register_nb_transport_bw(this, &Loader::nb_transport_bw_crossbar);

	for (unsigned int i = 0; i < m_pool_size+1; i++ )
	{
		m_transaction_queue.enqueue ();
	}

	SC_THREAD(read_data_thread);
	SC_THREAD(end_read_data_thread);
	SC_THREAD(send_thread);
	SC_THREAD(monitor_advance_comp_thread);
}

void Loader::write_context_reg(slc context)
{
	if(context.phid == m_ID)
	{
		m_ready = true;
		if(context.op_aux)
		{
			m_batch_len = pow(2,context.batch_len);
		}else
		{
			m_batch_len = context.batch_len;
		}
		m_addr_inc = context.addr_inc;
		m_addr = context.addr;
	}
}

void Loader::all_config()
{
	if(m_ready)
	{
		m_ready = false;	
		m_ini_done = true;
		m_data_cnt = 0;
		mAllInitiatedEvent.notify(sc_core::SC_ZERO_TIME);
	}
}

/**
 * @brief
 * @note: Date Loader <--> Data Memory
 * protocol: 4 phase 
 */
tlm::tlm_sync_enum 
Loader::nb_transport_bw_memory(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay)
{
	if(phase == tlm::END_REQ)
	{
		mEndRequestEvent.notify(sc_core::SC_ZERO_TIME);	
	}else if(phase == tlm::BEGIN_RESP)
	{
		delay += m_end_rsp_delay;
		mResponsePEQ.notify(trans, delay);
	}else 
	{
		assert(false && "loader.cpp: unrecognized pahse");
	}

	return tlm::TLM_ACCEPTED;
}

/**
 * @brief
 * @note: Date Loader <--> Crossbar
 */
tlm::tlm_sync_enum 
Loader::nb_transport_bw_crossbar(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay)
{
	if(phase == tlm::BEGIN_RESP)
	{
		mCompleteEvent.notify(sc_core::SC_ZERO_TIME);
	}else 
	{
		assert(false && "loader.cpp: unrecognized pahse");
	}
	return tlm::TLM_ACCEPTED;
}

/**
 * @brief
 * @note:
 * 最多支持4个滞外交易
 */
void Loader::read_data_thread()
{
	while(true)
	{
		wait(mAllInitiatedEvent);
		tlm::tlm_command	cmd		= tlm::TLM_READ_COMMAND;
		unsigned			len		= 4;
		while(m_data_cnt < m_batch_len)
		{
			mCheckAdvanceComEvent.notify(SC_ZERO_TIME);
			wait(mProceedEvent);
			m_advance_computing++;

			tlm::tlm_generic_payload	* trans_ptr;

			assert( !m_transaction_queue.is_empty());
			trans_ptr = m_transaction_queue.dequeue();
			trans_ptr->acquire();
			sc_dt::uint64		addr	= m_addr*4096 + (m_addr_counter++)*m_addr_inc*4;
			trans_ptr->set_command(cmd);
			trans_ptr->set_address(addr);
			trans_ptr->set_data_length(len);

			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
			tlm::tlm_sync_enum return_value;
		    return_value = isock_memory->nb_transport_fw(*trans_ptr, phase, delay);

			switch(return_value)
			{
				case tlm::TLM_ACCEPTED:
					wait(mEndRequestEvent); // ensure this transacton has been accepted and start next transaction
					cout << "### " << m_ID;
					cout << "Strating read data #" << m_data_cnt ;
					cout << "Addr: " << setw(5) << hex << addr;
					cout << " @ " << setw(5) << sc_time_stamp() << endl;
					break;
				default:
					std::cout << "ID: " << m_ID << "return_value " << return_value << std::endl;
					assert(false && "Loader.cpp: only 4 phase transactions supported now");
						
			}
			m_data_cnt++;
			if(m_data_cnt == m_batch_len)
			{
				m_ini_done = false;	
				m_addr_counter = 0;
			}
			wait(clock_period);
		}
	}
}
/**
 *
 */
void Loader::end_read_data_thread()
{
	while(true)
	{
		wait(mResponsePEQ.get_event());	
		tlm::tlm_generic_payload * trans_ptr;
		while((trans_ptr = mResponsePEQ.get_next_transaction())!=0)
		{
			tlm::tlm_phase phase  = tlm::END_RESP;          // set the appropriate phase
			sc_time delay         = SC_ZERO_TIME;

			tlm::tlm_sync_enum
				return_value = isock_memory->nb_transport_fw(*trans_ptr, phase, delay);

			switch (return_value)
			{
				case tlm::TLM_COMPLETED:                        // transaction complete
					{
						mIssuePEQ.notify(*trans_ptr, SC_ZERO_TIME);
						cout << "### " << m_ID;
						cout << setw(50) << "read data back @";
						cout << setw(5) << sc_time_stamp() << endl;
						break;
					}
				case tlm::TLM_ACCEPTED:
				case tlm::TLM_UPDATED:
				default:
					assert(false && " Unknown return value for END_RESP ");
					break;
			} // end switch
		} // end while
	}
}

void Loader::send_thread()
{
	while(true)
	{
		wait(mIssuePEQ.get_event());
		tlm::tlm_generic_payload * trans_ptr;
		while((trans_ptr = mIssuePEQ.get_next_transaction()) != 0)
		{

			cout << "### " << m_ID;
			cout << setw(100) << "Starting send data @";
			cout << setw(5) << sc_time_stamp() << endl;
			m_advance_computing--;
			mCheckAdvanceComEvent.notify(SC_ZERO_TIME);

			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			sc_time delay = SC_ZERO_TIME;
			tlm::tlm_sync_enum return_value;
			if( (!m_ini_done) & (m_advance_computing == 0))
			{
				trans_ptr->set_command(LAST_COMPUTE);	
			}
			return_value = isock_crossbar->nb_transport_fw(*trans_ptr, phase, delay);
			assert(return_value == tlm::TLM_ACCEPTED);

			wait(mCompleteEvent);
			cout << "### " << m_ID;
			cout << setw(150) << "End send data @";
			cout << setw(5) << sc_time_stamp() << endl;
	
			trans_ptr->release();
			if( (!m_ini_done) & (m_advance_computing == 0))
			{
				assert(slcs != 0 && "You should connect PE with RC");
				slcs->release_busy(m_ID);	
			}
		}	
	}
}

void Loader::monitor_advance_comp_thread()
{
	while(true)
	{
		wait(mCheckAdvanceComEvent);
		if(m_advance_computing < m_pool_size)
		{
			mProceedEvent.notify(SC_ZERO_TIME);
		}
	}
}

