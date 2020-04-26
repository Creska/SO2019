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

        unsigned int sys_n, arg1, arg2, arg3;                       // Retrieving syscall number and arguments from processor registers
        load_syscall_registers(interrupted_state, &sys_n, &arg1, &arg2, &arg3);

#ifdef TARGET_UMPS                                                  // TODO is this just for syscalls or also for breakpoints?
        interrupted_state->pc_epc += WORD_SIZE;
#endif

        DEBUG_LOG_INT("Exception recognised as syscall number ", sys_n);
        consume_syscall(sys_n, arg1, arg2, arg3, interrupted_state, interrupted_process);

    } else if (cause_code == EXCODE_BP) {
        DEBUG_LOG("Exception recognised as breakpoint");
        adderrbuf("ERROR: BreakPoint launched, handler still not implemented!");
    }

    flush_kernel_time(interrupted_process);

    if (interrupted_process == get_running_proc()) {
        DEBUG_LOG("Syscall/breakpoint handled, resuming the same process that was running before the exception");
        DEBUG_SPACING;
        LDST(interrupted_state);
    } else {
        DEBUG_LOG_INT("Syscall/breakpoint handled, resuming a different process that the one interrupted, with original priority: ", get_running_proc()->original_priority);
        DEBUG_SPACING;
        reset_int_timer();
        memcpy(&interrupted_process->p_s, interrupted_state, sizeof(state_t));          // Copies the old area state to the interrupted process state in order to guarantee that the information will be up to date when the process willl be resumed
        LDST(&get_running_proc()->p_s);                                                 // TODO we need to ensure that the interrupted process still exists and represents the same process (otherwise we would copy the info to an inactive pcb or even worst a pcb now used for something else)
    }                                                                                   // one possibility would be having a new field in pcb that is increased every time the pcb is allocated, this way we can check not only that the pid is the same, but also that this field is the same, meaning that the pcb still represents the same process
}


void handle_TLB() {
    DEBUG_LOG("HANDLING TLB EXCEPTION");
    adderrbuf("ERROR: TLB exception handling not implemented yet");
}


void handle_trap() {
    DEBUG_LOG("HANDLING PROGRAM TRAP EXCEPTION");
    adderrbuf("ERROR: program trap handling not implemented yet");
}
