#ifndef EA_INTERRUPT_GEN_IF_H
#define EA_INTERRUPT_GEN_IF_H
//BEGIN ea_interrupt_gen_if.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DESCRIPTION
//   This is a custom primitive channel that supports the notion of
//   sending events between modules. To send an event, simply invoke
//   notify with the appropriate arguments. The receiving modules should
//   wait on the default event.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ea_interrupt_gen_if: public sc_interface {
	public:
		virtual void notify() = 0;
		virtual void notify(sc_time t) = 0;
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: ea_interrupt_gen_if.h,v 1.1 2004/02/10 22:47:00 dcblack Exp $
