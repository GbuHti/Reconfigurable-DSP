#ifndef TERMINAL_H
#define TERMINAL_H

#include "systemc"

#include "tlm_utils/simple_target_socket.h"


struct Terminal : public sc_core::sc_module {

    tlm_utils::simple_target_socket<Terminal> tsock;

    Terminal(sc_core::sc_module_name) {
        tsock.register_b_transport(this, &Terminal::transport);
    }

    void transport(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) {
        sc_assert (trans.get_command() == tlm::TLM_WRITE_COMMAND);
        sc_assert (trans.get_data_length() == 1);

        std::cout << "Terminnal:" << (int)*trans.get_data_ptr() << std::endl;
//		std::cout << (char)*trans.get_data_ptr();
    }
};


#endif 
