#ifndef ISS_H
#define ISS_H

#include "instr.h"
#include "regfile.h"
#include "csr.h"
#include "mem_if.h"
#include "irq_if.h"
#include "clint_if.h"
#include "syscall.h"

#include "stdint.h"
#include "string.h"
#include "assert.h"
#include <unordered_set>

enum class CoreExecStatus {
    Runnable,
    HitBreakpoint,
    Terminated,
};


struct ISS : public sc_core::sc_module,
             public external_interrupt_target,
             public timer_interrupt_target {

    clint_if *clint;
    instr_memory_interface *instr_mem;
    data_memory_interface *mem;
    SyscallHandler *sys;
    RegFile regs;
    uint32_t pc;
    uint32_t last_pc;
    csr_table csrs;
    uint32_t lrw_marked = 0;

    CoreExecStatus status = CoreExecStatus::Runnable;
    std::unordered_set<uint32_t> breakpoints;
    bool debug_mode = false;

    sc_core::sc_event wfi_event;

//    tlm_utils::tlm_quantumkeeper quantum_keeper;
    sc_core::sc_time cycle_time;
    std::array<sc_core::sc_time, Opcode::NUMBER_OF_INSTRUCTIONS> instr_cycles;

    enum {
        REG_MIN = INT32_MIN,
    };

    ISS()
        : sc_module(sc_core::sc_module_name("ISS")) {

//        sc_core::sc_time qt = tlm::tlm_global_quantum::instance().get();
        cycle_time = sc_core::sc_time(10, sc_core::SC_NS);

//        assert (qt >= cycle_time);
//        assert (qt % cycle_time == sc_core::SC_ZERO_TIME);

        for (int i=0; i<Opcode::NUMBER_OF_INSTRUCTIONS; ++i)
            instr_cycles[i] = cycle_time;     // 所有指令的执行时间都是一个周期？-> 看下文

        const sc_core::sc_time memory_access_cycles = 4*cycle_time; //读/写延迟跟这个有什么关系？
        const sc_core::sc_time mul_div_cycles = 8*cycle_time;

        instr_cycles[Opcode::LB] = memory_access_cycles;
        instr_cycles[Opcode::LBU] = memory_access_cycles;
        instr_cycles[Opcode::LH] = memory_access_cycles;
        instr_cycles[Opcode::LHU] = memory_access_cycles;
        instr_cycles[Opcode::LW] = memory_access_cycles;
        instr_cycles[Opcode::SB] = memory_access_cycles;
        instr_cycles[Opcode::SH] = memory_access_cycles;
        instr_cycles[Opcode::SW] = memory_access_cycles;
        instr_cycles[Opcode::MUL] = mul_div_cycles;
        instr_cycles[Opcode::MULH] = mul_div_cycles;
        instr_cycles[Opcode::MULHU] = mul_div_cycles;
        instr_cycles[Opcode::MULHSU] = mul_div_cycles;
        instr_cycles[Opcode::DIV] = mul_div_cycles;
        instr_cycles[Opcode::DIVU] = mul_div_cycles;
        instr_cycles[Opcode::REM] = mul_div_cycles;
        instr_cycles[Opcode::REMU] = mul_div_cycles;
    }

    Opcode::mapping exec_step() {
        auto mem_word = instr_mem->load_instr(pc);
		sc_core::sc_time now = sc_time_stamp();
        Instruction instr(mem_word);
        Opcode::mapping op;
        if (instr.is_compressed()) {
            op = instr.decode_and_expand_compressed();
            pc += 2;
        } else {
            op = instr.decode_normal();
            pc += 4;
        }

        switch (op) {
            case Opcode::UNDEF:
                throw std::runtime_error("unknown instruction");

            case Opcode::ADDI:
                regs[instr.rd()] = regs[instr.rs1()] + instr.I_imm();
                break;

            case Opcode::SLTI:
                regs[instr.rd()] = regs[instr.rs1()] < instr.I_imm() ? 1 : 0;
                break;

            case Opcode::SLTIU:
                regs[instr.rd()] = ((uint32_t)regs[instr.rs1()]) < ((uint32_t)instr.I_imm()) ? 1 : 0;
                break;

            case Opcode::XORI:
                regs[instr.rd()] = regs[instr.rs1()] ^ instr.I_imm();
                break;

            case Opcode::ORI:
                regs[instr.rd()] = regs[instr.rs1()] | instr.I_imm();
                break;

            case Opcode::ANDI:
                regs[instr.rd()] = regs[instr.rs1()] & instr.I_imm();
                break;

            case Opcode::ADD:
                regs[instr.rd()] = regs[instr.rs1()] + regs[instr.rs2()];
                break;

            case Opcode::SUB:
                regs[instr.rd()] = regs[instr.rs1()] - regs[instr.rs2()];
                break;

            case Opcode::SLL:
                regs[instr.rd()] = regs[instr.rs1()] << regs.shamt(instr.rs2());
                break;

            case Opcode::SLT:
                regs[instr.rd()] = regs[instr.rs1()] < regs[instr.rs2()];
                break;

            case Opcode::SLTU:
                regs[instr.rd()] = ((uint32_t)regs[instr.rs1()]) < ((uint32_t)regs[instr.rs2()]);
                break;

            case Opcode::SRL:
                regs[instr.rd()] = ((uint32_t)regs[instr.rs1()]) >> regs.shamt(instr.rs2());
                break;

            case Opcode::SRA:
                regs[instr.rd()] = regs[instr.rs1()] >> regs.shamt(instr.rs2());
                break;

            case Opcode::XOR:
                regs[instr.rd()] = regs[instr.rs1()] ^ regs[instr.rs2()];
                break;

            case Opcode::OR:
                regs[instr.rd()] = regs[instr.rs1()] | regs[instr.rs2()];
                break;

            case Opcode::AND:
                regs[instr.rd()] = regs[instr.rs1()] & regs[instr.rs2()];
                break;

            case Opcode::SLLI:
                regs[instr.rd()] = regs[instr.rs1()] << instr.shamt();
                break;

            case Opcode::SRLI:
                regs[instr.rd()] = ((uint32_t)regs[instr.rs1()]) >> instr.shamt();
                break;

            case Opcode::SRAI:
                regs[instr.rd()] = regs[instr.rs1()] >> instr.shamt();
                break;

            case Opcode::LUI:
                regs[instr.rd()] = instr.U_imm();
                break;

            case Opcode::AUIPC:
                regs[instr.rd()] = last_pc + instr.U_imm();
                break;

            case Opcode::JAL:
                if (instr.rd() != RegFile::zero)
                    regs[instr.rd()] = pc;
                pc = last_pc + instr.J_imm();
                break;

            case Opcode::JALR: {
                uint32_t link = pc;
                pc = (regs[instr.rs1()] + instr.I_imm()) & ~1;
                if (instr.rd() != RegFile::zero)
                    regs[instr.rd()] = link;
            }
                break;

            case Opcode::SB: {
                uint32_t addr = regs[instr.rs1()] + instr.S_imm();
                mem->store_byte(addr, regs[instr.rs2()]);
            }
                break;

            case Opcode::SH: {
                uint32_t addr = regs[instr.rs1()] + instr.S_imm();
                mem->store_half(addr, regs[instr.rs2()]);
            }
                break;

            case Opcode::SW: {
                uint32_t addr = regs[instr.rs1()] + instr.S_imm();
                mem->store_word(addr, regs[instr.rs2()]);
            }
                break;

            case Opcode::LB: {
                uint32_t addr = regs[instr.rs1()] + instr.I_imm();
                regs[instr.rd()] = mem->load_byte(addr);
            }
                break;

            case Opcode::LH: {
                uint32_t addr = regs[instr.rs1()] + instr.I_imm();
                regs[instr.rd()] = mem->load_half(addr);
            }
                break;

            case Opcode::LW: {
                uint32_t addr = regs[instr.rs1()] + instr.I_imm();
                regs[instr.rd()] = mem->load_word(addr);
            }
                break;

            case Opcode::LBU: {
                uint32_t addr = regs[instr.rs1()] + instr.I_imm();
                regs[instr.rd()] = mem->load_ubyte(addr);
            }
                break;

            case Opcode::LHU: {
                uint32_t addr = regs[instr.rs1()] + instr.I_imm();
                regs[instr.rd()] = mem->load_uhalf(addr);
            }
                break;

            case Opcode::BEQ:
                if (regs[instr.rs1()] == regs[instr.rs2()])
                    pc = last_pc + instr.B_imm();
                break;

            case Opcode::BNE:
                if (regs[instr.rs1()] != regs[instr.rs2()])
                    pc = last_pc + instr.B_imm();
                break;

            case Opcode::BLT:
                if (regs[instr.rs1()] < regs[instr.rs2()])
                    pc = last_pc + instr.B_imm();
                break;

            case Opcode::BGE:
                if (regs[instr.rs1()] >= regs[instr.rs2()])
                    pc = last_pc + instr.B_imm();
                break;

            case Opcode::BLTU:
                if ((uint32_t)regs[instr.rs1()] < (uint32_t)regs[instr.rs2()])
                    pc = last_pc + instr.B_imm();
                break;

            case Opcode::BGEU:
                if ((uint32_t)regs[instr.rs1()] >= (uint32_t)regs[instr.rs2()])
                    pc = last_pc + instr.B_imm();
                break;

            case Opcode::FENCE: {
                // not using out of order execution so can be ignored
                break;
            }

            case Opcode::ECALL: {
                // NOTE: cast to unsigned value to avoid sign extension, since execute_syscall expects a native 64 bit value
                int ans = sys->execute_syscall((uint32_t)regs[RegFile::a7], (uint32_t)regs[RegFile::a0], (uint32_t)regs[RegFile::a1], (uint32_t)regs[RegFile::a2], (uint32_t)regs[RegFile::a3]);
                regs[RegFile::a0] = ans;
            } break;

            case Opcode::EBREAK:
                status = CoreExecStatus::HitBreakpoint;
                break;

            case Opcode::CSRRW: {
                auto rd = instr.rd();
                auto rs1_val = regs[instr.rs1()];
                auto &csr = csr_update_and_get(instr.csr());
                if (rd != RegFile::zero) {
                    regs[instr.rd()] = csr.read();
                }
                csr.write(rs1_val);
            } break;

            case Opcode::CSRRS: {
                auto rd = instr.rd();
                auto rs1 = instr.rs1();
                auto rs1_val = regs[instr.rs1()];
                auto &csr = csr_update_and_get(instr.csr());
                if (rd != RegFile::zero)
                    regs[rd] = csr.read();
                if (rs1 != RegFile::zero)
                    csr.set_bits(rs1_val);
            } break;

            case Opcode::CSRRC: {
                auto rd = instr.rd();
                auto rs1 = instr.rs1();
                auto rs1_val = regs[instr.rs1()];
                auto &csr = csr_update_and_get(instr.csr());
                if (rd != RegFile::zero)
                    regs[rd] = csr.read();
                if (rs1 != RegFile::zero)
                    csr.clear_bits(rs1_val);
            } break;

            case Opcode::CSRRWI: {
                auto rd = instr.rd();
                auto &csr = csr_update_and_get(instr.csr());
                if (rd != RegFile::zero) {
                    regs[rd] = csr.read();
                }
                csr.write(instr.zimm());
            } break;

            case Opcode::CSRRSI: {
                auto rd = instr.rd();
                auto zimm = instr.zimm();
                auto &csr = csr_update_and_get(instr.csr());
                if (rd != RegFile::zero)
                    regs[rd] = csr.read();
                if (zimm != 0)
                    csr.set_bits(zimm);
            } break;

            case Opcode::CSRRCI: {
                auto rd = instr.rd();
                auto zimm = instr.zimm();
                auto &csr = csr_update_and_get(instr.csr());
                if (rd != RegFile::zero)
                    regs[rd] = csr.read();
                if (zimm != 0)
                    csr.clear_bits(zimm);
            } break;


            case Opcode::MUL: {
                int64_t ans = (int64_t)regs[instr.rs1()] * (int64_t)regs[instr.rs2()];
                regs[instr.rd()] = ans & 0xFFFFFFFF;
            } break;

            case Opcode::MULH: {
                int64_t ans = (int64_t)regs[instr.rs1()] * (int64_t)regs[instr.rs2()];
                regs[instr.rd()] = (ans & 0xFFFFFFFF00000000) >> 32;
            } break;

            case Opcode::MULHU: {
                int64_t ans = ((uint64_t)(uint32_t)regs[instr.rs1()]) * (uint64_t)((uint32_t)regs[instr.rs2()]);
                regs[instr.rd()] = (ans & 0xFFFFFFFF00000000) >> 32;
            } break;

            case Opcode::MULHSU: {
                int64_t ans = (int64_t)regs[instr.rs1()] * (uint64_t)((uint32_t)regs[instr.rs2()]);
                regs[instr.rd()] = (ans & 0xFFFFFFFF00000000) >> 32;
            } break;

            case Opcode::DIV: {
                auto a = regs[instr.rs1()];
                auto b = regs[instr.rs2()];
                if (b == 0) {
                    regs[instr.rd()] = -1;
                } else if (a == REG_MIN && b == -1) {
                    regs[instr.rd()] = a;
                } else {
                    regs[instr.rd()] = a / b;
                }
            } break;

            case Opcode::DIVU: {
                auto a = regs[instr.rs1()];
                auto b = regs[instr.rs2()];
                if (b == 0) {
                    regs[instr.rd()] = -1;
                } else {
                    regs[instr.rd()] = (uint32_t)a / (uint32_t)b;
                }
            } break;

            case Opcode::REM: {
                auto a = regs[instr.rs1()];
                auto b = regs[instr.rs2()];
                if (b == 0) {
                    regs[instr.rd()] = a;
                } else if (a == REG_MIN && b == -1) {
                    regs[instr.rd()] = 0;
                } else {
                    regs[instr.rd()] = a % b;
                }
            } break;

            case Opcode::REMU: {
                auto a = regs[instr.rs1()];
                auto b = regs[instr.rs2()];
                if (b == 0) {
                    regs[instr.rd()] = a;
                } else {
                    regs[instr.rd()] = (uint32_t)a % (uint32_t)b;
                }
            } break;


            case Opcode::LR_W: {
                //TODO: in multi-threaded system (or even if other components can access the memory independently, e.g. through DMA) need to mark this addr as reserved
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                assert (addr != 0);
                lrw_marked = addr;
            } break;

            case Opcode::SC_W: {
                uint32_t addr = regs[instr.rs1()];
                uint32_t val  = regs[instr.rs2()];
                //TODO: check if other components (besides this iss) may have accessed the last marked memory region
                if (lrw_marked == addr) {
                    mem->store_word(addr, val);
                    regs[instr.rd()] = 0;
                } else {
                    regs[instr.rd()] = 1;
                }
                lrw_marked = 0;
            } break;

            //TODO: implement the aq and rl flags if necessary (check for all AMO instructions)
            case Opcode::AMOSWAP_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                mem->store_word(addr, regs[instr.rs2()]);
            } break;

            case Opcode::AMOADD_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                uint32_t val = regs[instr.rd()] + regs[instr.rs2()];
                mem->store_word(addr, val);
            } break;

            case Opcode::AMOXOR_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                uint32_t val = regs[instr.rd()] ^ regs[instr.rs2()];
                mem->store_word(addr, val);
            } break;

            case Opcode::AMOAND_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                uint32_t val = regs[instr.rd()] & regs[instr.rs2()];
                mem->store_word(addr, val);
            } break;

            case Opcode::AMOOR_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                uint32_t val = regs[instr.rd()] | regs[instr.rs2()];
                mem->store_word(addr, val);
            } break;

            case Opcode::AMOMIN_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                uint32_t val = std::min(regs[instr.rd()], regs[instr.rs2()]);
                mem->store_word(addr, val);
            } break;

            case Opcode::AMOMINU_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                uint32_t val = std::min((uint32_t)regs[instr.rd()], (uint32_t)regs[instr.rs2()]);
                mem->store_word(addr, val);
            } break;

            case Opcode::AMOMAX_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                uint32_t val = std::max(regs[instr.rd()], regs[instr.rs2()]);
                mem->store_word(addr, val);
            } break;

            case Opcode::AMOMAXU_W: {
                uint32_t addr = regs[instr.rs1()];
                regs[instr.rd()] = mem->load_word(addr);
                uint32_t val = std::max((uint32_t)regs[instr.rd()], (uint32_t)regs[instr.rs2()]);
                mem->store_word(addr, val);
            } break;


            case Opcode::WFI:
                //NOTE: only a hint, can be implemented as NOP
                //std::cout << "[sim:wfi] CSR mstatus.mie " << csrs.mstatus->mie << std::endl;
                if (!has_pending_enabled_interrupts())
                    sc_core::wait(wfi_event);
                break;

            case Opcode::SFENCE_VMA:
                //NOTE: not using MMU so far, so can be ignored
                break;

            case Opcode::URET:
            case Opcode::SRET:
                assert (false && "not implemented");
            case Opcode::MRET:
                return_from_trap_handler();
                break;

            default:
                assert (false && "unknown opcode");
        }

        //NOTE: writes to zero register are supposedly allowed but must be ignored (reset it after every instruction, instead of checking *rd != zero* before every register write)
        regs.regs[regs.zero] = 0;

        return op;
    }


	/**
	 * @brief 计算当前的运行周期
	 * 计算方法：周期数 = 运行时间/周期时间
	 * @param 无
	 * @return uint64_t
	 * @note 运行时间 = local time + sc_time_stamp()
	 */
    uint64_t _compute_and_get_current_cycles() {
        // Note: result is based on the default time resolution of SystemC (1 PS)
        sc_core::sc_time now = sc_core::sc_time_stamp(); 

        assert (now % cycle_time == sc_core::SC_ZERO_TIME);
        assert (now.value() % cycle_time.value() == 0);

        uint64_t num_cycles = now.value() / cycle_time.value();

        return num_cycles;
    }

	/** 
	 * @brief 访问CSR寄存器
	 * @param CSR寄存器地址
	 * @return csr_base &
	 * 当访问一些实时更新的CSR寄存器时，需要先更新，然后再送出
	 */
    csr_base &csr_update_and_get(uint32_t addr) {
        switch (addr) {
            case CSR_TIME_ADDR:
            case CSR_MTIME_ADDR: {
                uint64_t mtime = clint->update_and_get_mtime();
                csrs.time_root->reg = mtime;
                return *csrs.time;
            }

            case CSR_TIMEH_ADDR:
            case CSR_MTIMEH_ADDR: {
                uint64_t mtime = clint->update_and_get_mtime();
                csrs.time_root->reg = mtime;
                return *csrs.timeh;
            }

            case CSR_MCYCLE_ADDR:
                csrs.cycle_root->reg = _compute_and_get_current_cycles();
                return *csrs.cycle;

            case CSR_MCYCLEH_ADDR:
                csrs.cycle_root->reg = _compute_and_get_current_cycles();
                return *csrs.cycleh;

            case CSR_MINSTRET_ADDR:
                return *csrs.instret;

            case CSR_MINSTRETH_ADDR:
                return *csrs.instreth;
        }

        return csrs.at(addr);
    }


    void init(instr_memory_interface *instr_mem, data_memory_interface *data_mem, clint_if *clint,
              SyscallHandler *sys, uint32_t entrypoint, uint32_t sp) {
        this->instr_mem = instr_mem;
        this->mem = data_mem;
        this->clint = clint; // 猜一波CLINT的对象
        this->sys = sys;
        regs[RegFile::sp] = sp;
        pc = entrypoint;
        csrs.setup();
    }

    virtual void trigger_external_interrupt() override {
        //std::cout << "[sim] trigger external interrupt" << std::endl;
        csrs.mip->meip = true;
        wfi_event.notify(sc_core::SC_ZERO_TIME);
    }

    virtual void trigger_timer_interrupt(bool status) override {
        csrs.mip->mtip = status;
        wfi_event.notify(sc_core::SC_ZERO_TIME);
    }

    void return_from_trap_handler() {
        //std::cout << "[sim] return from trap handler @time " << quantum_keeper.get_current_time() << " to pc " << std::hex << csrs.mepc->reg << std::endl;

        // NOTE: assumes a SW based solution to store/re-store the execution context, since this appears to be the RISC-V convention
        pc = csrs.mepc->reg;

        // NOTE: need to adapt when support for privilege levels beside M-mode is added
        csrs.mstatus->mie = csrs.mstatus->mpie;
        csrs.mstatus->mpie = 1;
    }

    bool has_pending_enabled_interrupts() {
        assert (!csrs.mip->msip && "traps and syscalls are handled in the simulator");

        return csrs.mstatus->mie && ((csrs.mie->meie && csrs.mip->meip) || (csrs.mie->mtie && csrs.mip->mtip));
    }

    void switch_to_trap_handler() {
        assert (csrs.mstatus->mie);
        //std::cout << "[sim] switch to trap handler @time " << quantum_keeper.get_current_time() << " @last_pc " << std::hex << last_pc << " @pc " << pc << std::endl;

        csrs.mcause->interrupt = 1;
        if (csrs.mie->meie && csrs.mip->meip) {
            csrs.mcause->exception_code = 11;
        } else if (csrs.mie->mtie && csrs.mip->mtip) {
            csrs.mcause->exception_code = 7;
        } else {
            assert (false);     // enabled pending interrupts must be available if this function is called
        }

        // for SW traps the address of the instruction causing the trap/interrupt (i.e. last_pc, the address of the ECALL,EBREAK - better set pc=last_pc before taking trap)
        // for interrupts the address of the next instruction to execute (since e.g. the RISC-V FreeRTOS port will not modify it)
        csrs.mepc->reg = pc;

        // deactivate interrupts before jumping to trap handler (SW can re-activate if supported)
        csrs.mstatus->mpie = csrs.mstatus->mie;
        csrs.mstatus->mie = 0;

        // perform context switch to trap handler
        pc = csrs.mtvec->get_base_address();
    }


    void performance_and_sync_update(Opcode::mapping executed_op) {
        ++csrs.instret_root->reg;

        auto new_cycles = instr_cycles[executed_op];

		wait(new_cycles);
//        quantum_keeper.inc(new_cycles);
//        if (quantum_keeper.need_sync()) { ///< 判断时间片是否已经用完
//            quantum_keeper.sync();        ///< 同步，将多个已经完成的transaction的delay形成的local time同步掉。
//        }
    }

    void run_step() {
        assert (regs.read(0) == 0);

        //std::cout << "pc: " << std::hex << pc << " sp: " << regs.read(regs.sp) << std::endl;
        last_pc = pc;
        Opcode::mapping op = exec_step();

        if (has_pending_enabled_interrupts())
            switch_to_trap_handler();

        // Do not use a check *pc == last_pc* here. The reason is that due to interrupts *pc* can be set to *last_pc* accidentally (when jumping back to *mepc*).
        if (sys->shall_exit)
            status = CoreExecStatus::Terminated;

        // speeds up the execution performance (non debug mode) significantly by checking the additional flag first
        if (debug_mode && (breakpoints.find(pc) != breakpoints.end()))
            status = CoreExecStatus::HitBreakpoint;

        performance_and_sync_update(op);
		sc_core::sc_time now = sc_time_stamp();
    }

    void run() {
        // run a single step until either a breakpoint is hit or the execution terminates
        do {
            run_step();
        } while (status == CoreExecStatus::Runnable);

        // force sync to make sure that no action is missed
        //quantum_keeper.sync();
    }

    void show() {
        std::cout << "simulation time: " << sc_core::sc_time_stamp() << std::endl;
        regs.show();
        std::cout << "pc = " << pc << std::endl;
        std::cout << "num-instr = " << csrs.instret_root->reg << std::endl;
        std::cout << "max-heap (c-lib malloc, bytes) = " << sys->get_max_heap_memory_consumption() << std::endl;
    }
};


/* Do not call the run function of the ISS directly but use one of the Runner wrappers. */
struct DirectCoreRunner : public sc_core::sc_module {

    ISS &core;

    SC_HAS_PROCESS(DirectCoreRunner);

    DirectCoreRunner(ISS &core)
        : sc_module(sc_core::sc_module_name("DirectCoreRunner")), core(core) {
        SC_THREAD(run);
    }

    void run() {
        core.run();

        if (core.status == CoreExecStatus::HitBreakpoint) {
            throw std::runtime_error("Breakpoints are not supported in the direct runner, use the debug runner instead.");
        }
        assert (core.status == CoreExecStatus::Terminated);

        sc_core::sc_stop();
    }
};
#endif
