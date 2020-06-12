#include "core/exceptions/handler.h"
#include "utils/debug.h"
#include "utils/utils.h"
#include "core/system/system.h"
#include "core/exceptions/syscalls.h"
#include "core/exceptions/interrupts.h"
#include "core/processes/scheduler.h"
#include "core/processes/asl.h"






state_t *get_spec_area(int area_type, int exc_type, pcb_t* proc) {
    return proc->spec_areas[exc_type*2 + area_type];
}

int is_passup_set(unsigned int type, pcb_t* p) {
    state_t* old_area = p->spec_areas[type*2];
    state_t* new_area = p->spec_areas[type*2+1];

    return ((old_area)!=NULL && (new_area)!=NULL);
}

// Timer utility functions ----------------------------------------------------

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

// During an handler this pointer is set to the PCB that was running while the exception was raised
pcb_t* interrupted_proc;

void start_handler() {
    interrupted_proc = get_running_proc();
    flush_user_time(interrupted_proc);
}
void conclude_handler(unsigned int exc_type) {
    pcb_t* resuming_proc = get_running_proc();
    if (interrupted_proc != resuming_proc) {
        reset_cached_tod(resuming_proc);
        memcpy(&interrupted_proc->p_s, GET_AREA(AREA_TYPE_OLD, exc_type), sizeof(state_t));
        LDST(&resuming_proc->p_s);                                             // Resume the execution of the changed process

    } else {
        flush_kernel_time(interrupted_proc);
        LDST(GET_AREA(AREA_TYPE_OLD, exc_type));                                       // Resume the execution of the same process that was interrupted, retrieving it from the old area
    }
}

void handle_interrupt() {
    DEBUG_LOG_UINT("HANDLING INTERRUPT EXCEPTIONS during proc ", get_process_index(get_running_proc()));
    start_handler();

    consume_interrupts();                                               // this way we can know if the interrupt consumption changed the process running (see below)
    reset_int_timer();                                                  // reset the interval timer acknowledging the interrupt and guaranteeing a full time-slice for the process that will be resumed

    DEBUG_SPACING;
    conclude_handler(EXC_TYPE_INT);                                                                 // this allows us to memcpy state_t information only when we swap processes
}


void launch_spec_area(int exc_type, state_t* interrupted_state) {
    state_t* spec_old_area = get_spec_area(AREA_TYPE_OLD, exc_type, get_running_proc());
    memcpy(spec_old_area, interrupted_state, sizeof(state_t));
    state_t* spec_new_area = get_spec_area(AREA_TYPE_NEW, exc_type, get_running_proc());
    LDST(spec_new_area);
}


void handle_sysbreak() {

    pcb_t* interrupted_process = get_running_proc();          // Cache the running process before handling to avoid excessive use of memcpy (see handle_interrupts comments)
    DEBUG_LOG_UINT("HANDLING SYSCALL/BREAKPOINT EXCEPTION during proc ", get_process_index(interrupted_process));

    flush_user_time(interrupted_process);
    state_t *interrupted_state = GET_AREA(AREA_TYPE_OLD, EXC_TYPE_SYS);

#ifdef TARGET_UMPS
    interrupted_state->pc_epc += WORD_SIZE;
#endif

    unsigned int cause_code = get_exccode(interrupted_state);

    if (cause_code == EXCODE_SYS) {
        if (*sys_n(interrupted_state) > 8) {
            if (is_passup_set(EXC_TYPE_SYS, interrupted_process)) {
                DEBUG_LOG("SYS PASSUP!");
                launch_spec_area(EXC_TYPE_SYS, interrupted_state);
            } else {
                DEBUG_LOG_INT("No syscall vector, terminating process", get_process_index(get_running_proc()));
                terminate_proc(interrupted_process);
            }
        } else {
            consume_syscall(interrupted_state, interrupted_process);
            DEBUG_LOG("Syscall handled");
        }

    } else if (cause_code == EXCODE_BP) {
        DEBUG_LOG("Exception recognised as breakpoint");
        if (is_passup_set(EXC_TYPE_SYS, interrupted_process)) {
            launch_spec_area(EXC_TYPE_SYS, interrupted_state);
        } else {
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
        DEBUG_LOG_UINT("Resuming a different process than the one that was interrupted: ", get_process_index(get_running_proc()));
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
    if (is_passup_set(EXC_TYPE_TLB, get_running_proc())) {
        DEBUG_LOG("Spec areas set");
        state_t* old_area = get_spec_area(AREA_TYPE_OLD, EXC_TYPE_TLB, get_running_proc());
        memcpy(old_area, GET_AREA(AREA_TYPE_OLD, EXC_TYPE_TLB), sizeof(state_t));
        LDST(get_spec_area(AREA_TYPE_NEW, EXC_TYPE_TLB, get_running_proc()));
    }
    // TODO does this needs to be killed?
    terminate_proc(get_running_proc());
    LDST(&get_running_proc()->p_s);
}


void handle_trap() {
    //check_passup(EXC_TYPE_PRG, get_running_proc());
    DEBUG_LOG("HANDLING PROGRAM TRAP EXCEPTION");
    if (is_passup_set(EXC_TYPE_PRG, get_running_proc())) {
        DEBUG_LOG("Spec areas set");
        state_t* old_area = get_spec_area(AREA_TYPE_OLD, EXC_TYPE_PRG, get_running_proc());
        memcpy(old_area, GET_AREA(AREA_TYPE_OLD, EXC_TYPE_PRG), sizeof(state_t));
        LDST(get_spec_area(AREA_TYPE_NEW, EXC_TYPE_PRG, get_running_proc()));
    }

    terminate_proc(get_running_proc());
    LDST(&get_running_proc()->p_s);
}
