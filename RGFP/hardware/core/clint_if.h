#ifndef CLINT_IF_H
#define CLINT_IF_H

#include "irq_if.h"
#include "device.h"

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
	public Device,
	public sc_core::sc_module
{
	static constexpr uint64_t scaler = 1000000;
	sc_core::sc_time clock_cycle = sc_core::sc_time(10, sc_core::SC_NS);
	sc_event irq_event;

	sc_core::sc_time m_clint_delay;
	
	timer_interrupt_target * m_target;	

	SC_HAS_PROCESS(Clint);

	Clint
	(sc_core::sc_module_name
	 ,sc_core::sc_time clint_delay
	 ,timer_interrupt_target * target
	)
	:m_clint_delay(clint_delay)
	,m_target(target)
	{		
		addr_to_reg = {
			{0xbff8, (uint32_t *)&mtime},
			{0xbffc, (uint32_t *)&mtime + 1},
			{0x4000, (uint32_t *)&mtimecmp},
			{0x4004, (uint32_t *)&mtimecmp + 1},
		};

		SC_THREAD(run);
	}

	void operation(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) override;

	sc_core::sc_time get_device_delay(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) override;

	uint64_t update_and_get_mtime() override;

	void run();


	private:
		uint64_t mtime;
		uint64_t mtimecmp;
		std::map<uint64_t, uint32_t *> addr_to_reg;
		
};

#endif
