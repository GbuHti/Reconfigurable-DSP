#ifndef CLINT_IF_H
#define CLINT_IF_H

#include "irq_if.h"

#include "systemc.h"
#include "tlm_utils/simple_target_socket.h"

#include <map>

/**
 * @brief Core Local Interrupt Controller, 生成计时器中断
 */
struct clint_if
{
	~clint_if() 
	{
	}

	virtual uint64_t update_and_get_mtime() = 0;
};

struct Clint: public clint_if, 
	public sc_core::sc_module
{
	static constexpr uint64_t scaler = 1000000;
	sc_core::sc_time clock_cycle = sc_core::sc_time(10, sc_core::SC_NS);
	sc_event irq_event;

	tlm_utils::simple_target_socket<Clint> tsock;
	
	timer_interrupt_target * m_target;	

	SC_HAS_PROCESS(Clint);

	Clint(sc_core::sc_module_name, timer_interrupt_target * target)
		:m_target(target)
	{		
		tsock.register_b_transport(this, &Clint::b_transport);
		addr_to_reg = {
			{0xbff8, (uint32_t *)&mtime},
			{0xbffc, (uint32_t *)&mtime + 1},
			{0x4000, (uint32_t *)&mtimecmp},
			{0x4004, (uint32_t *)&mtimecmp + 1},
		};
		
		SC_THREAD(run);
	}

	uint64_t update_and_get_mtime() override;

	void run();

	void b_transport(tlm::tlm_generic_payload &pg, sc_core::sc_time &delay);

	private:
		uint64_t mtime;
		uint64_t mtimecmp;
		std::map<uint64_t, uint32_t *> addr_to_reg;
		
};

#endif
