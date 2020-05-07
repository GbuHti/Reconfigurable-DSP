#ifndef MASTER_H
#define MASTER_H

#include "systemc.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_get.h"

#include "contextreg_if.h"

#define GENERATE_ARITH_PE_SLC(lgid,op,mux_a,mux_b) \
	(lgid<<27) + (op<<24) + (mux_a<<19) + (mux_b<<14) 


using namespace std;
using namespace sc_core;

class Master: public sc_module
{
	private:
		unsigned m_ID;
		tlm::tlm_generic_payload m_trans;
		unsigned m_data;

		sc_event mEndResponseEvent;
	public:
		tlm_utils::simple_initiator_socket<Master> isock;

		SC_HAS_PROCESS(Master);
		Master
		( sc_module_name name
		, unsigned id
		)
		:sc_module(name)
		, m_ID(id)
		{
			isock.register_nb_transport_bw(this, &Master::nb_transport_bw);
			SC_THREAD(request_thread);
		}

		void request_thread()
		{
			tlm::tlm_phase phase = tlm::BEGIN_REQ;
			sc_time delay = SC_ZERO_TIME;
			m_trans.set_data_ptr((unsigned char *)&m_data);
			tlm::tlm_sync_enum return_value;
			for(int i = 0; i< 4; i++)
			{
				if(i == 2) m_trans.set_command(tlm::TLM_WRITE_COMMAND);
				cout << "master " << m_ID << " send transacton!!!!" << endl;
				m_data = i;
				delay = SC_ZERO_TIME;
				return_value = isock->nb_transport_fw(m_trans,phase,delay);	
				assert(return_value == tlm::TLM_ACCEPTED);
				wait(mEndResponseEvent);
				cout << "master " << m_ID << " got response! @" << sc_time_stamp() << endl;
				cout << endl;
			}
		}

		tlm::tlm_sync_enum
			nb_transport_bw
			( tlm::tlm_generic_payload &trans
			, tlm::tlm_phase &phase
			, sc_time &delay
			 )
			{
				assert( phase == tlm::BEGIN_RESP);
				mEndResponseEvent.notify(SC_ZERO_TIME);
				return tlm::TLM_ACCEPTED;
			}
};

class Slaver : public sc_module
{
	private:
		unsigned m_ID;
		unsigned char m_port;
		sc_time m_accept_delay;
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mResponsePEQ;
		unsigned trans_num;
	public:
		tlm_utils::simple_target_socket<Slaver> tsock;

		SC_HAS_PROCESS(Slaver);
		Slaver
		( sc_module_name name
		, unsigned id
		, unsigned char port
		, sc_time accept_delay
		)
		: sc_module(name)
		, m_ID(id)
		, m_port(port)
		, m_accept_delay(accept_delay)
		, mResponsePEQ("mResponsePEQ")
		, trans_num(0)
		{
			tsock.register_nb_transport_fw(this, &Slaver::nb_transport_fw);
			SC_THREAD(response_thread);	
		}

		void response_thread()
		{
			while(true)
			{
				wait(mResponsePEQ.get_event());
				tlm::tlm_generic_payload * trans_ptr;
				while( (trans_ptr = mResponsePEQ.get_next_transaction()) != 0)
				{
					tlm::tlm_phase phase = tlm::BEGIN_RESP;
					sc_time delay = SC_ZERO_TIME;
					tlm::tlm_sync_enum return_value;
					cout << "slaver " << m_ID << m_port << " response " << trans_num << " @" << sc_time_stamp() << endl;
					return_value = tsock->nb_transport_bw(*trans_ptr, phase, delay);
					assert(return_value == tlm::TLM_ACCEPTED);

				}
			}

		}
		
		tlm::tlm_sync_enum
			nb_transport_fw
			( tlm::tlm_generic_payload &trans
			, tlm::tlm_phase &phase
			, sc_time &delay
			)
			{
				trans_num++;
				cout << "salver " << m_ID << m_port << " got transaction " << trans_num << " @" << sc_time_stamp() << endl;
				mResponsePEQ.notify(trans, m_accept_delay);
				return tlm::TLM_ACCEPTED;	
			}
		
};

#endif


