#include "core/exceptions/handler.h"
#include "utils/debug.h"
#include "core/system/system.h"
#include "core/exceptions/syscalls.h"
#include "core/exceptions/interrupts.h"
#include "core/processes/scheduler.h"


void handle_interrupt() {
    DEBUG_LOG("HANDLING INTERRUPT EXCEPTIONS");
    pcb_t* interrupted_running_proc = get_running_proc();               // We cache the running process before consuming interrupts
    consume_interrupts();                                               // this way we can know if the interrupt consumption changed the process running (see below)
    reset_int_timer();                                                  // reset the interval timer acknowledging the interrupt and guaranteeing a full time-slice for the process that will be resumed
    DEBUG_SPACING;
    if (interrupted_running_proc!=get_running_proc()) {
        LDST(&get_running_proc()->p_s);                                 // Resume the execution of the changed process
    } else {
        LDST(get_old_area_int());                                       // Resume the execution of the same process that was interrupted, retrieving it from the old area
    }                                                                   // this allows us to memcpy state_t information only when we swap processes
}


void handle_sysbreak() {

    DEBUG_LOG("HANDLING SYSCALL/BREAKPOINT EXCEPTION");

    unsigned int cause_code = get_exccode(get_old_area_sys_break());
    state_t* s = get_old_area_sys_break();
    pcb_t* pre_handling_running_proc = get_running_proc();          // Cache the running process before handling to avoid excessive use of memcpy (see handle_interrupts comments)

    if (cause_code == EXCODE_SYS) {

        unsigned int sys_n, arg1, arg2, arg3;                       // Retrieving syscall number and arguments from processor registers
        load_syscall_registers(s, &sys_n, &arg1, &arg2, &arg3);

#ifdef TARGET_UMPS                      // TODO is this just for syscalls or also for breakpoints?
        s->pc_epc += WORD_SIZE;
#endif

        DEBUG_LOG_INT("Exception recognised as syscall number ", sys_n);

        switch (sys_n) {                                    // Using a switch since this will handle a few different syscalls in the next phases
            case SYSCALL_TERMINATE_PROC: {
                terminate_running_proc();
                break;
            }
            case SYSCALL_ADD_PROC: {
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

    if (pre_handling_running_proc==get_running_proc()) {
        DEBUG_LOG("Syscall/breakpoint handled, resuming the same process that was running before the exception");
        DEBUG_SPACING;
        LDST(s);
    } else {
        DEBUG_LOG_INT("Syscall/breakpoint handled, resuming a different process that the one interrupted, with original priority: ", get_running_proc()->original_priority);
        DEBUG_SPACING;
        LDST(&get_running_proc()->p_s);
    }
}


void handle_TLB() {
    DEBUG_LOG("HANDLING TLB EXCEPTION");
    adderrbuf("ERROR: TLB exception handling not implemented yet");
}


void handle_trap() {
    DEBUG_LOG("HANDLING PROGRAM TRAP EXCEPTION");
    adderrbuf("ERROR: program trap handling not implemented yet");
}
