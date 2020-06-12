#include "core/exceptions/handler.h"
#include "utils/debug.h"
#include "utils/utils.h"
#include "core/system/system.h"
#include "core/exceptions/syscalls.h"
#include "core/exceptions/interrupts.h"
#include "core/processes/scheduler.h"
#include "core/processes/asl.h"


// During an handler this pointer is set to the PCB that was running while the exception was raised
pcb_t* interrupted_proc;


// Passup areas utility functions -------------------------------------------------------------------------------------

state_t *get_spec_area(enum area_age area_age, enum exc_type area_type, pcb_t* proc) {
    return proc->spec_areas[area_type*2 + area_age];
}

int is_passup_set(enum exc_type area_type, pcb_t* p) {
    state_t* old_area = p->spec_areas[area_type*2];
    state_t* new_area = p->spec_areas[area_type*2+1];

    return ((old_area)!=NULL && (new_area)!=NULL);
}

void launch_spec_area(int exc_type, state_t* interrupted_state) {
    state_t* spec_old_area = get_spec_area(OLD, exc_type, get_running_proc());
    memcpy(spec_old_area, interrupted_state, sizeof(state_t));
    state_t* spec_new_area = get_spec_area(OLD, exc_type, get_running_proc());
    LDST(spec_new_area);
}

// Timer utility functions --------------------------------------------------------------------------------------------

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

// Handler routines ---------------------------------------------------------------------------------------------------


// TODO check useless int timer resets (now the handler resets the timer when at the end of the interrupt the process has changed, verify that the scheduler doesn't do this when the handler will do it either way)


void start_handler() {
    interrupted_proc = get_running_proc();
    flush_user_time(interrupted_proc);
}

void conclude_handler(enum exc_type exc_type) {
    pcb_t* resuming_proc = get_running_proc();

    debug_ready_queue();
    debug_asl();
    DEBUG_SPACING;

    if (interrupted_proc != resuming_proc) {
        reset_cached_tod(resuming_proc);
        memcpy(&interrupted_proc->p_s, GET_AREA(OLD, exc_type), sizeof(state_t));
        LDST(&resuming_proc->p_s);                                             // Resume the execution of the changed process
    } else {
        flush_kernel_time(interrupted_proc);
        LDST(GET_AREA(OLD, exc_type));                                       // Resume the execution of the same process that was interrupted, retrieving it from the old area
    }
}

// HANDLER FUNCTIONS --------------------------------------------------------------------------------------------------

void handle_interrupt() {
    DEBUG_LOG_UINT("HANDLING INTERRUPT EXCEPTIONS during proc ", get_process_index(get_running_proc()));
    start_handler();

    consume_interrupts();
    reset_int_timer();                                       // reset the interval timer acknowledging the interrupt and guaranteeing a full time-slice for the process that will be resumed

    conclude_handler(INT);
}





void handle_sysbreak() {

    DEBUG_LOG_UINT("HANDLING SYSCALL/BREAKPOINT EXCEPTION during proc ", get_process_index(interrupted_process));
    start_handler();

    state_t *interrupted_state = GET_AREA(OLD, SYS);
#ifdef TARGET_UMPS
    interrupted_state->pc_epc += WORD_SIZE;
#endif

    unsigned int cause_code = get_exccode(interrupted_state);
    if (cause_code == EXCODE_SYS) {
        if (*sys_n(interrupted_state) > 8) {
            if (is_passup_set(SYS, interrupted_proc)) {
                DEBUG_LOG("SYS PASSUP!");
                launch_spec_area(SYS, interrupted_state);
            } else {
                DEBUG_LOG_INT("No syscall vector, terminating process", get_process_index(get_running_proc()));
                terminate_proc(interrupted_proc);
            }
        } else {
            consume_syscall(interrupted_state, interrupted_proc);
            DEBUG_LOG("Syscall handled");
        }
    } else if (cause_code == EXCODE_BP) {
        DEBUG_LOG("Exception recognised as breakpoint");
        if (is_passup_set(SYS, interrupted_proc)) {
            launch_spec_area(SYS, interrupted_state);
        } else {
            adderrbuf("ERROR: BreakPoint launched, handler still not implemented!");
        }
    }

    flush_kernel_time(interrupted_proc);
    conclude_handler(SYS);
}


void handle_TLB() {
    DEBUG_LOG("HANDLING TLB EXCEPTION");
    if (is_passup_set(TLB, get_running_proc())) {
        DEBUG_LOG("Spec areas set");

        state_t* old_area = get_spec_area(OLD, TLB, get_running_proc());
        memcpy(old_area, GET_AREA(OLD, TLB), sizeof(state_t));
        LDST(get_spec_area(NEW, TLB, get_running_proc()));
    }
    // TODO does this needs to be killed?
    terminate_proc(get_running_proc());
    LDST(&get_running_proc()->p_s);
}


void handle_trap() {
    //check_passup(EXC_TYPE_PRG, get_running_proc());
    DEBUG_LOG("HANDLING PROGRAM TRAP EXCEPTION");
    if (is_passup_set(PRG, get_running_proc())) {
        DEBUG_LOG("Spec areas set");
        state_t* old_area = get_spec_area(OLD, PRG, get_running_proc());
        memcpy(old_area, GET_AREA(OLD, PRG), sizeof(state_t));
        LDST(get_spec_area(NEW, PRG, get_running_proc()));
    }

    terminate_proc(get_running_proc());
    LDST(&get_running_proc()->p_s);
}
