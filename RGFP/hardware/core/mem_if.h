#ifndef MEM_IF
#define MEM_IF

#include <iostream>

#include "systemc.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"

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

struct CombinedMemoryInterface : public sc_core::sc_module,
                                 public instr_memory_interface,
                                 public data_memory_interface {
    typedef uint32_t addr_t;

    tlm_utils::simple_initiator_socket<CombinedMemoryInterface> isock;
    tlm_utils::tlm_quantumkeeper &quantum_keeper;

    CombinedMemoryInterface(sc_core::sc_module_name, tlm_utils::tlm_quantumkeeper &keeper)
        : quantum_keeper(keeper) {
    }

    inline void _do_transaction(tlm::tlm_command cmd, uint64_t addr, uint8_t *data, unsigned num_bytes) {
        tlm::tlm_generic_payload trans;
        trans.set_command(cmd);
        trans.set_address(addr);
        trans.set_data_ptr(data);
        trans.set_data_length(num_bytes);

        sc_core::sc_time local_delay = quantum_keeper.get_local_time();

        isock->b_transport(trans, local_delay);

        assert (local_delay >= quantum_keeper.get_local_time()); // 保证local只能递增，不能递减
        quantum_keeper.set(local_delay);
    }

    template <typename T>
    inline T _load_data(addr_t addr) {
        T ans;
        _do_transaction(tlm::TLM_READ_COMMAND, addr, (uint8_t*)&ans, sizeof(T));
        return ans;
    }

    template <typename T>
    inline void _store_data(addr_t addr, T value) {
        _do_transaction(tlm::TLM_WRITE_COMMAND, addr, (uint8_t*)&value, sizeof(T));
    }

    int32_t load_instr(addr_t addr) { return _load_data<int32_t>(addr); }

    int32_t load_word(addr_t addr) { return _load_data<int32_t>(addr); }
    int32_t load_half(addr_t addr) { return _load_data<int16_t>(addr); }
    int32_t load_byte(addr_t addr) { return _load_data<int8_t>(addr); }
    uint32_t load_uhalf(addr_t addr) { return _load_data<uint16_t>(addr); }
    uint32_t load_ubyte(addr_t addr) { return _load_data<uint8_t>(addr); }

    void store_word(addr_t addr, uint32_t value) { _store_data(addr, value); }
    void store_half(addr_t addr, uint16_t value) { _store_data(addr, value); }
    void store_byte(addr_t addr, uint8_t value) { _store_data(addr, value); }
};

#endif
