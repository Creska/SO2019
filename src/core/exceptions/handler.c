#include "core/exceptions/handler.h"
#include "utils/debug.h"
#include "utils/utils.h"
#include "core/system/system.h"
#include "core/exceptions/syscalls.h"
#include "core/exceptions/interrupts.h"
#include "core/processes/scheduler.h"
#include "core/processes/asl.h"

#define BIKAYA_RESERVED_SPACE 0x20000800

#define INTERRUPTED_PROC BIKAYA_RESERVED_SPACE


#define EXC_TYPE_SYS    0
#define EXC_TYPE_TLB    1
#define EXC_TYPE_PRG    2

#define AREA_TYPE_OLD   0
#define AREA_TYPE_NEW   1

state_t *get_spec_area(int area_type, int exc_type, pcb_t* proc) {
    return proc->spec_areas[exc_type*2 + area_type];
}

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

    pcb_t* interrupted_running_proc = get_running_proc();               // We cache the running process before consuming interrupts
    DEBUG_LOG_UINT("HANDLING INTERRUPT EXCEPTIONS during proc ", get_process_index(interrupted_running_proc));
    flush_user_time(interrupted_running_proc);
    consume_interrupts();                                               // this way we can know if the interrupt consumption changed the process running (see below)
    reset_int_timer();                                                  // reset the interval timer acknowledging the interrupt and guaranteeing a full time-slice for the process that will be resumed
    DEBUG_SPACING;
    pcb_t* running_proc_after_consume = get_running_proc();

    debug_ready_queue();
    debug_asl();

    if (interrupted_running_proc!=running_proc_after_consume) {
        reset_cached_tod(running_proc_after_consume);
        memcpy(&interrupted_running_proc->p_s, get_old_area_int(), sizeof(state_t));
        LDST(&running_proc_after_consume->p_s);                                             // Resume the execution of the changed process

    } else {
        flush_kernel_time(interrupted_running_proc);
        LDST(get_old_area_int());                                       // Resume the execution of the same process that was interrupted, retrieving it from the old area
    }                                                                   // this allows us to memcpy state_t information only when we swap processes
}


int is_passup_set(unsigned int type, pcb_t* p) {
    state_t* old_area = p->spec_areas[type*2];
    state_t* new_area = p->spec_areas[type*2+1];

    return ((old_area)!=NULL && (new_area)!=NULL);
}




void handle_sysbreak() {

    pcb_t* interrupted_process = get_running_proc();          // Cache the running process before handling to avoid excessive use of memcpy (see handle_interrupts comments)
    *(pcb_t**)INTERRUPTED_PROC = interrupted_process;

    DEBUG_LOG_UINT("HANDLING SYSCALL/BREAKPOINT EXCEPTION during proc ", get_process_index(interrupted_process));
    flush_user_time(interrupted_process);
    state_t* interrupted_state = get_old_area_sys_break();
    DEBUG_LOG_INT("Sys n: ", *sys_n(interrupted_state));

#ifdef TARGET_UMPS                                                  // TODO is this just for syscalls or also for breakpoints?
    interrupted_state->pc_epc += WORD_SIZE;
#endif

    if (*sys_n(interrupted_state) > 8) {
        if (is_passup_set(EXC_TYPE_SYS, interrupted_process)) {
            DEBUG_LOG("SYS PASSUP!");
            state_t* spec_old_area = get_spec_area(AREA_TYPE_OLD, EXC_TYPE_SYS, interrupted_process);
            memcpy(spec_old_area, get_old_area_sys_break(), sizeof(state_t));

            state_t* spec_new_area = get_spec_area(AREA_TYPE_NEW, EXC_TYPE_SYS, interrupted_process);
            *sys_n(spec_old_area) = *sys_n(interrupted_state);
            LDST(spec_new_area);
        } else {
            DEBUG_LOG_INT("No syscall vector, terminating process", get_process_index(get_running_proc()));
            terminate_proc(get_running_proc());
        }

    } else {


        unsigned int cause_code = get_exccode(get_old_area_sys_break());

        if (cause_code == EXCODE_SYS) {


            consume_syscall(interrupted_state, interrupted_process);
            DEBUG_LOG("Syscall handled");


        } else if (cause_code == EXCODE_BP) {
            DEBUG_LOG("Exception recognised as breakpoint");
            adderrbuf("ERROR: BreakPoint launched, handler still not implemented!");
        }

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
        DEBUG_LOG_INT("Resuming a different process than the one that was interrupted: ", get_process_index(get_running_proc()));
        memcpy(&interrupted_process->p_s, interrupted_state, sizeof(state_t));                      // Copies the old area state to the interrupted process state in order to guarantee that the information will be up to date when the process willl be resumed
        to_resume= &get_running_proc()->p_s;
        reset_int_timer();
    }

    if (&get_idle_proc()->p_s==to_resume) {                    //TEMP
        DEBUG_LOG("Resuming idle process");
    }

    flush_kernel_time(interrupted_process);

    debug_ready_queue();
    debug_asl();

    DEBUG_LOG("LDST");
    DEBUG_SPACING;

    LDST(to_resume);
}


void handle_TLB() {
    DEBUG_LOG("HANDLING TLB EXCEPTION");
    if (is_passup_set(EXC_TYPE_SYS, get_running_proc())) {
        state_t* old_area = get_spec_area(AREA_TYPE_OLD, EXC_TYPE_TLB, get_running_proc());
        memcpy(old_area, get_old_area_TLB(), sizeof(state_t));
        LDST(get_spec_area(AREA_TYPE_NEW, EXC_TYPE_TLB, get_running_proc()));
    }

    adderrbuf("ERROR: TLB exception handling not implemented yet");
}


void handle_trap() {
    //check_passup(EXC_TYPE_PRG, get_running_proc());
    DEBUG_LOG("HANDLING PROGRAM TRAP EXCEPTION");
    if (is_passup_set(EXC_TYPE_SYS, get_running_proc())) {
        state_t* old_area = get_spec_area(AREA_TYPE_OLD, EXC_TYPE_PRG, get_running_proc());
        memcpy(old_area, get_old_area_program_trap(), sizeof(state_t));
        LDST(get_spec_area(AREA_TYPE_NEW, EXC_TYPE_PRG, get_running_proc()));
    }

    terminate_proc(get_running_proc());
    LDST(&get_running_proc()->p_s);
    adderrbuf("ERROR: program trap handling not implemented yet");
}
