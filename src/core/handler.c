#include "core/exceptions/handler.h"
#include "utils/debug.h"
#include "core/exceptions/interrupts.h"
#include "core/system/system.h"
#include "core/processes/scheduler.h"

void handle_interrupt() {
    DEBUG_LOG("HANDLING INTERRUPTS");
    pcb_t* interrupted_running_proc = get_running_proc();               // We cache the running process before consuming interrupts
    consume_interrupts();                                               // this way we can know if the interrupt consumption changed the process running (see below)
    DEBUG_SPACING;
    reset_interval_timer();                                             // reset the interval timer (acknowledging the interrupt)
    if (interrupted_running_proc!=get_running_proc()) {
        LDST(&get_running_proc()->p_s);                                 // Resume the execution of the changed process
    } else {
        LDST(get_old_area_int());                                       // Resume the execution of the same process that was interrupted, retrieving it from the old area
    }                                                                   // this allows us to memcpy state_t information only when we swap processes
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

        unsigned int sys_n, arg1, arg2, arg3;                       // Retrieving syscall number and arguments from processor registers
        load_syscall_registers(s, &sys_n, &arg1, &arg2, &arg3);

#ifdef TARGET_UMPS                      // TODO is this just for syscalls or also for breakpoints?
        s->pc_epc += WORD_SIZE;
#endif


        DEBUG_LOG_INT("Exception recognised as syscall number ", sys_n);

        pcb_t* proc_to_resume;
        switch (sys_n) {                                    // Using a switch since this will handle a few different syscalls
            case 3: {
                terminate_running_proc();
                proc_to_resume = get_running_proc();
                DEBUG_LOG_INT("Resuming process with original priority", proc_to_resume->original_priority);
                reset_interval_timer();                     // since this is a "new" process might as well give it a full time-slice
                LDST(&proc_to_resume->p_s);
                break;
            }
            case 20: {
                add_process((proc_init_data*)arg1);
                break;
            }
            default: {
                adderrbuf("ERROR: Syscall not implemented");
                break;
            }
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
