#ifndef ARTITH_PE_H
#define ARITH_PE_H

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_get.h"

#include "contextreg_if.h"
#include "define.h"
#include "slcs_if.h"

#include <queue>

using namespace std;
using namespace sc_core;

class Arith_PE : public sc_module
			   , public contextreg_if
{
	private:
		unsigned m_ID;
		unsigned m_op;
		unsigned m_op_aux;
		bool m_binocular;
		bool m_ini_done;
		unsigned m_pool_size;

		unsigned m_advance_computing;
		bool m_waiting;

		float m_srca;
		float m_srcb;

		sc_event mCheckAdvanceComEvent;
		sc_event mProceedEvent;
		sc_event mExecuteEvent;
		
		sc_event mBeginResponseEvent;
		
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mSendPEQ;
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mSrc_a_PEQ;
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mSrc_b_PEQ;

	public:
		slcs_if * slcs; ///< the interface between Loader and Reconfiguration Controller

		tlm_utils::simple_target_socket_tagged<Arith_PE> tsock[2];
		tlm_utils::simple_initiator_socket<Arith_PE> isock;

		Arith_PE
		( sc_module_name name
		, unsigned id
		, bool binocular
		);

		tlm::tlm_sync_enum 
		nb_transport_fw
		( int id
		, tlm::tlm_generic_payload &trans
		, tlm::tlm_phase &phase
		, sc_time &delay
		);

		tlm::tlm_sync_enum 
		nb_transport_bw
		( tlm::tlm_generic_payload &trans
		, tlm::tlm_phase &phase
		, sc_time &delay
		);
	
		void wait_data_thread();

		void execute_thread();

		void send_thread();

		void monitor_advance_comp_thread();

		void write_context_reg(slc context) override;

		void all_config() override;

		//{{{ tlm memory manager interface
		static const unsigned int  m_txn_data_size = 4;         // transaction size
		class tg_queue_c                                        /// memory managed queue class
			: public tlm::tlm_mm_interface                          /// implements memory management IF
		{
			public:

				tg_queue_c                                            /// tg_queue_c constructor
					( void
					)
					{
					}

				void
					enqueue                                                /// enqueue entry (create)
					( void
					)
					{ 
						tlm::tlm_generic_payload  *transaction_ptr  = new tlm::tlm_generic_payload ( this ); /// transaction pointer
						unsigned char             *data_buffer_ptr  = new unsigned char [ m_txn_data_size ];  /// data buffer pointer

						transaction_ptr->set_data_ptr ( data_buffer_ptr );

						m_queue.push ( transaction_ptr );
					}

				tlm::tlm_generic_payload *                            /// transaction pointer
					dequeue                                               /// dequeue entry
					( void
					)
					{
						tlm::tlm_generic_payload *transaction_ptr = m_queue.front ();

						m_queue.pop();

						return transaction_ptr;
					}


				void
					release                                               /// release entry
					( tlm::tlm_generic_payload *transaction_ptr           /// transaction pointer
					)
					{
						transaction_ptr->release ();
					}

				bool                                                  /// true / false
					is_empty                                              /// queue empty
					( void
					)
					{
						return m_queue.empty ();
					}

				size_t                                                /// queue size
					size                                                  /// queue size
					( void
					)
					{
						return m_queue.size ();
					}

				void
					free                                                  /// return transaction
					( tlm::tlm_generic_payload *transaction_ptr           /// to the pool
					)
					{
						transaction_ptr->reset();
						m_queue.push ( transaction_ptr );
					}

			private:

				std::queue<tlm::tlm_generic_payload*> m_queue;        /// queue
		};
		//}}}

		tg_queue_c		m_transaction_queue;

};


#endif
