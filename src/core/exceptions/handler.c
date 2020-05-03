#include "core/exceptions/handler.h"
#include "utils/debug.h"
#include "utils/utils.h"
#include "core/system/system.h"
#include "core/exceptions/syscalls.h"
#include "core/exceptions/interrupts.h"
#include "core/processes/scheduler.h"


void flush_user_time(pcb_t* proc) {
    unsigned int cached_TOD = TOD;
    proc->user_timer += cached_TOD - proc->tod_cache;
    proc->tod_cache = cached_TOD;
}

void flush_kernel_time(pcb_t* proc) {
    unsigned int cached_TOD = TOD;
    proc->kernel_timer += cached_TOD - proc->tod_cache;
    proc->tod_cache = cached_TOD;
}

void reset_cached_tod(pcb_t* proc) {
    proc->tod_cache = TOD;
}

// TODO check useless int timer resets (now the handler resets the timer when at the end of the interrupt the process has changed, verify that the scheduler doesn't do this when the handler will do it either way)


void handle_interrupt() {
    DEBUG_LOG("HANDLING INTERRUPT EXCEPTIONS");

    pcb_t* interrupted_running_proc = get_running_proc();               // We cache the running process before consuming interrupts
    flush_user_time(interrupted_running_proc);
    consume_interrupts();                                               // this way we can know if the interrupt consumption changed the process running (see below)
    reset_int_timer();                                                  // reset the interval timer acknowledging the interrupt and guaranteeing a full time-slice for the process that will be resumed
    DEBUG_SPACING;
    pcb_t* running_proc_after_consume = get_running_proc();


    if (interrupted_running_proc!=running_proc_after_consume) {
        reset_cached_tod(running_proc_after_consume);
        memcpy(&interrupted_running_proc->p_s, get_old_area_int(), sizeof(state_t));
        reset_int_timer();
        LDST(&running_proc_after_consume->p_s);                                             // Resume the execution of the changed process

    } else {
        flush_kernel_time(interrupted_running_proc);
        LDST(get_old_area_int());                                       // Resume the execution of the same process that was interrupted, retrieving it from the old area
    }                                                                   // this allows us to memcpy state_t information only when we swap processes
}


void handle_sysbreak() {

    DEBUG_LOG("HANDLING SYSCALL/BREAKPOINT EXCEPTION");
    pcb_t* interrupted_process = get_running_proc();          // Cache the running process before handling to avoid excessive use of memcpy (see handle_interrupts comments)
    flush_user_time(interrupted_process);
    unsigned int cause_code = get_exccode(get_old_area_sys_break());
    state_t* interrupted_state = get_old_area_sys_break();

    if (cause_code == EXCODE_SYS) {

#ifdef TARGET_UMPS                                                  // TODO is this just for syscalls or also for breakpoints?
        interrupted_state->pc_epc += WORD_SIZE;
#endif

        consume_syscall(interrupted_state, interrupted_process);
        DEBUG_LOG("Syscall handled");

    } else if (cause_code == EXCODE_BP) {
        DEBUG_LOG("Exception recognised as breakpoint");
        adderrbuf("ERROR: BreakPoint launched, handler still not implemented!");
    }



    state_t* to_resume = NULL;
    if (interrupted_process == get_running_proc()) {
        if (get_running_proc()->user_timer==0) {
            DEBUG_LOG("The interrupted process was terminated and it's PCB was recycled for a new process that "
                      "now needs to be resumed. This shouldn't happen with the currently available syscalls");
            to_resume = &get_running_proc()->p_s;
            reset_int_timer();
        } else {
            DEBUG_LOG("Resuming the interrupted process");
            to_resume = interrupted_state;
        }
    } else {
        DEBUG_LOG("Resuming a different process than the one that was interrupted");
        memcpy(&interrupted_process->p_s, interrupted_state, sizeof(state_t));                      // Copies the old area state to the interrupted process state in order to guarantee that the information will be up to date when the process willl be resumed
        to_resume= &get_running_proc()->p_s;
        reset_int_timer();
    }
    flush_kernel_time(interrupted_process);
    LDST(to_resume);
}


void handle_TLB() {
    DEBUG_LOG("HANDLING TLB EXCEPTION");
    adderrbuf("ERROR: TLB exception handling not implemented yet");
}


void handle_trap() {
    DEBUG_LOG("HANDLING PROGRAM TRAP EXCEPTION");
    adderrbuf("ERROR: program trap handling not implemented yet");
}
