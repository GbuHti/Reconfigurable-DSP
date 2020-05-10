#ifndef STORER_H
#define STORER_H

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

class Storer : public sc_module
			 , public contextreg_if
{
	private:
		unsigned m_ID;
		unsigned m_op;
		unsigned m_op_aux;
		unsigned m_addr_inc;
		unsigned m_addr;

		bool m_ini_done;
		unsigned m_pool_size;
		unsigned m_addr_counter;
		unsigned m_advance_computing;

		float m_result;

		sc_event mCheckAdvanceComEvent;
		sc_event mProceedEvent;
		sc_event mSendDataEvent;
		sc_event mBeginResponseEvent;
	
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mResultPEQ;
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mBufferPEQ;
	public:
		slcs_if * slcs; ///< the interface between Loader and Reconfiguration Controller

		tlm_utils::simple_target_socket<Storer> tsock;
		tlm_utils::simple_initiator_socket<Storer> isock;

		Storer
		( sc_module_name name
		, unsigned id
		);	

		tlm::tlm_sync_enum 
		nb_transport_fw
		( tlm::tlm_generic_payload &trans
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

		void fill_buffer_thread();

		void send_data_thread();

		void monitor_pool_thread();

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
