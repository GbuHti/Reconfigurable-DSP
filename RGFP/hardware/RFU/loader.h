#ifndef LOADER_H
#define LOADER_H

#include "contextreg_if.h"
#include "slcs_if.h"

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_get.h"

#include <queue>
#include "define.h"

class Loader:public sc_core::sc_module
			 ,public contextreg_if
{
	private:
		unsigned m_ID;
		unsigned m_batch_len;
		unsigned m_addr_inc;
		unsigned m_addr;

		sc_core::sc_time m_end_rsp_delay;

		unsigned m_data_cnt;

		bool m_ready;
		bool m_ini_done;
		unsigned m_pool_size;
		unsigned m_addr_counter;
		unsigned m_advance_computing;

		sc_core::sc_event mAllInitiatedEvent;
		sc_core::sc_event mEndRequestEvent;
		sc_core::sc_event mCompleteEvent;
		
		sc_core::sc_event mCheckAdvanceComEvent;
		sc_core::sc_event mProceedEvent;

		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mResponsePEQ;
		tlm_utils::peq_with_get<tlm::tlm_generic_payload> mIssuePEQ;

	public:
		tlm_utils::simple_initiator_socket<Loader> isock_memory;
		tlm_utils::simple_initiator_socket<Loader> isock_crossbar;

		slcs_if * slcs; ///< the interface between Loader and Reconfiguration Controller

		Loader
		( sc_core::sc_module_name name
		,unsigned ID
		,sc_core::sc_time end_rsp_delay
		);

		void read_data_thread();

		void end_read_data_thread();

		void send_thread();

		void monitor_advance_comp_thread();

		void write_context_reg(slc context) override;
		void all_config() override;

		tlm::tlm_sync_enum 
		nb_transport_bw_memory
		( tlm::tlm_generic_payload	&trans
		, tlm::tlm_phase			&pahse
		, sc_core::sc_time			&delay
		);

		tlm::tlm_sync_enum 
		nb_transport_bw_crossbar
		( tlm::tlm_generic_payload	&trans
		, tlm::tlm_phase			&pahse
		, sc_core::sc_time			&delay
		);

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


