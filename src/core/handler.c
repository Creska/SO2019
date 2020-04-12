#include "core/exceptions/handler.h"
#include "utils/debug.h"
#include "core/exceptions/interrupts.h"
#include "core/system/system.h"
#include "core/processes/scheduler.h"

// TODO check increment/decrement on SO group

void handle_interrupt() {
    DEBUG_LOG("HANDLING INTERRUPTS");
    consume_interrupts();
    DEBUG_LOG("");
    set_interval_timer(get_ticks_per_slice());          // reset the interval timer (acknowledging the interrupt)
    LDST(&get_running_proc()->p_s);                                 // Resume the execution of the process
}

void load_syscall_registers(state_t* s, unsigned int* n, unsigned int* a1, unsigned int* a2, unsigned int* a3) {
#ifdef TARGET_UMPS                                  // (handled with ifdef for now to avoid useless complexity, in the next phase this kind of stuff could be handled with a pattern similar to system.h)
    *n = s->reg_a0;
    *a1 = s->reg_a1;
    *a2 = s->reg_a2;
    *a3 = s->reg_a3;
#elif TARGET_UARM
    *n = s->a1;
    *a1 = s->a2;
    *a2 = s->a3;
    *a3 = s->a4;
#endif
}

void handle_sysbreak() {

    DEBUG_LOG("HANDLING SYSCALL/BREAKPOINT");

    unsigned int cause_code = get_exccode(get_old_area_sys_break());
    state_t* s = get_old_area_sys_break();

    if (cause_code == EXCODE_SYS) {

        unsigned int sys_n, arg1, arg2, arg3;                // Retrieving syscall number and arguments from processor registers
        load_syscall_registers(s, &sys_n, &arg1, &arg2, &arg3);

#ifdef TARGET_UMPS                      // TODO is this just for syscalls or also for breakpoints?
        s->pc_epc += WORD_SIZE;
#endif


        DEBUG_LOG_INT("Exception recognised as syscall ", sys_n);

        switch (sys_n) {                               // Using a switch since this will handle a few different syscalls
            case 3: {
                state_t* proc_to_resume = syscall3();
                DEBUG_LOG("Resuming higher priority process after removal\n");
                LDST(proc_to_resume);                       // TODO should a process be resumed with a full time-slice?
                break;
            }
            default:
                adderrbuf("ERROR: Syscall not implemented");
        }

    } else if (cause_code == EXCODE_BP) {
        DEBUG_LOG("Exception recognised as breakpoint");
        adderrbuf("ERROR: BreakPoint launched, handler still not implemented!");
    }

    DEBUG_LOG("Resuming the process that was interrupted by the syscall/breakpoint exception. (In phase 1.5 we should never reach this point)");
    LDST(s);
}

void handle_TLB() {
    adderrbuf("ERROR: TLB exception handling not implemented yet");
}

void handle_trap() {
    adderrbuf("ERROR: program trap handling not implemented yet");
}
