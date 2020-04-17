#ifndef MEM_IF
#define MEM_IF

#include <iostream>
#include <queue>

#include "systemc.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_get.h"

struct instr_memory_interface
{
	virtual ~instr_memory_interface()
	{
	}	

	virtual int32_t load_instr(uint32_t pc) = 0;
};

struct data_memory_interface
{
	virtual ~data_memory_interface()
	{
	}	

	virtual int32_t load_word(uint32_t addr) = 0;
	virtual int32_t load_half(uint32_t addr) = 0;
	virtual int32_t load_byte(uint32_t addr) = 0;
	virtual uint32_t load_uhalf(uint32_t addr) = 0;
	virtual uint32_t load_ubyte(uint32_t addr) = 0;

	virtual void store_word(uint32_t addr, uint32_t value) = 0;
	virtual void store_half(uint32_t addr, uint16_t value) = 0;
	virtual void store_byte(uint32_t addr, uint8_t value) = 0;
};

struct CombinedMemoryInterface : public sc_core::sc_module
	,public instr_memory_interface
	,public data_memory_interface 
{
	typedef uint32_t addr_t;

	tlm_utils::simple_initiator_socket<CombinedMemoryInterface> isock;
	sc_core::sc_event mEndRequestEvent;
	sc_core::sc_event mEndResponseEvent;
	tlm_utils::peq_with_get<tlm::tlm_generic_payload> m_send_end_rsp_PEQ;
	sc_core::sc_time m_end_rsp_delay;

	CombinedMemoryInterface
		(sc_core::sc_module_name name
		 , sc_core::sc_time end_rsp_delay
		 , unsigned int max_txns
		);

	//Used as begin request thread
	void _do_transaction(tlm::tlm_command cmd, uint64_t addr, uint8_t *data, unsigned num_bytes);

	void end_response_thread();

	tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &delay);

	/**
	 * @brief _load_data
	 */
	template <typename T>
		inline T _load_data(addr_t addr) 
		//{{{
		{
			T ans;
			_do_transaction(tlm::TLM_READ_COMMAND, addr, (uint8_t*)&ans, sizeof(T));
			wait(mEndResponseEvent); //不用等到response阶段结束，已经假设ldst的执行时间足够长
			return ans;
		}
	//}}}

	/**
	 * @brief _store_data
	 */
	template <typename T>
		inline void _store_data(addr_t addr, T value) 
		//{{{
		{
			_do_transaction(tlm::TLM_WRITE_COMMAND, addr, (uint8_t*)&value, sizeof(T));
		}
	//}}}

	//{{{ functions: lw lh lb lhu lbu sw sh sb
	int32_t load_instr(addr_t addr) { return _load_data<int32_t>(addr); }

	int32_t load_word(addr_t addr) { return _load_data<int32_t>(addr); }
	int32_t load_half(addr_t addr) { return _load_data<int16_t>(addr); }
	int32_t load_byte(addr_t addr) { return _load_data<int8_t>(addr); }
	uint32_t load_uhalf(addr_t addr) { return _load_data<uint16_t>(addr); }
	uint32_t load_ubyte(addr_t addr) { return _load_data<uint8_t>(addr); }

	void store_word(addr_t addr, uint32_t value) { _store_data(addr, value); }
	void store_half(addr_t addr, uint16_t value) { _store_data(addr, value); }
	void store_byte(addr_t addr, uint8_t value) { _store_data(addr, value); }

	//}}}
	
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
	unsigned int	m_active_txn_count;

};


#endif
