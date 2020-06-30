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

// Cache for the interval timer set at the beginning of an handler and used to restore the interval
// timer value at the end of the handler (when appropriate)
unsigned int int_timer_cache;


// Passup areas utility functions -------------------------------------------------------------------------------------

// Returns a pointer to the passup area of a given process
state_t *get_passup_area(enum area_age area_age, enum exc_type area_type, pcb_t* proc) {
    return proc->spec_areas[area_type*2 + area_age];
}

// Returns 1 if the passup areas (old AND new) are set for a given process and for a given exception type.
int is_passup_set(enum exc_type area_type, pcb_t* p) {
    state_t* old_area = p->spec_areas[area_type*2];
    state_t* new_area = p->spec_areas[area_type*2+1];
    return ((old_area)!=NULL && (new_area)!=NULL);
}

// Executes the passup area of the running process of the given exception type.
void launch_spec_area(int exc_type, state_t* interrupted_state) {
    state_t* spec_old_area = get_passup_area(OLD, exc_type, get_running_proc());
    memcpy(spec_old_area, interrupted_state, sizeof(state_t));
    state_t* spec_new_area = get_passup_area(NEW, exc_type, get_running_proc());
    LDST(spec_new_area);
}

// Timer utility functions --------------------------------------------------------------------------------------------

// Handler routines ---------------------------------------------------------------------------------------------------

void start_handler() {
    int_timer_cache = get_interval_timer_macro();
    if (int_timer_cache > get_clock_ticks_per_time_slice()) { int_timer_cache = get_clock_ticks_per_time_slice(); }
    interrupted_proc = get_running_proc();
    flush_user_time(interrupted_proc);
}

void conclude_handler(enum exc_type exc_type) {

    debug_ready_queue();
    debug_asl();
    DEBUG_SPACING;

    pcb_t* resuming_proc = get_running_proc();
    if (interrupted_proc != resuming_proc) {
        reset_cached_tod(resuming_proc);            // Don't count the kernel time if we're swapping processes
        reset_int_timer();
        if (interrupted_proc!=get_idle_proc())
            memcpy(&interrupted_proc->p_s, GET_AREA(OLD, exc_type), sizeof(state_t));
        LDST(&resuming_proc->p_s);
    } else {
        set_interval_timer(int_timer_cache);
        state_t* interrupted_state = GET_AREA(OLD, exc_type);
        flush_kernel_time(interrupted_proc);
        LDST(interrupted_state);                                       // Resume the execution of the same process that was interrupted, retrieving it from the old area
    }
}

// HANDLER FUNCTIONS --------------------------------------------------------------------------------------------------

void handle_interrupt() {
    DEBUG_LOG_UINT("HANDLING INTERRUPT EXCEPTIONS during proc ", get_process_index(get_running_proc()));
    start_handler();

    consume_interrupts();

    conclude_handler(INT);
}


void handle_sysbreak() {

    DEBUG_LOG_UINT("HANDLING SYSCALL/BREAKPOINT EXCEPTION during proc ", get_process_index(interrupted_proc));
    start_handler();

    state_t *interrupted_state = GET_AREA(OLD, SYS);
#ifdef TARGET_UMPS
    interrupted_state->pc_epc += WORD_SIZE;
#endif

    enum exc_code cause_code = get_exccode(interrupted_state);
    if (cause_code == E_SYS) {
        if (*sys_n(interrupted_state) > 8) {
            if (is_passup_set(SYS, interrupted_proc)) {
                flush_kernel_time(interrupted_proc);
                launch_spec_area(SYS, interrupted_state);
            } else { terminate_proc(interrupted_proc); }
        } else {
            consume_syscall(interrupted_state, interrupted_proc);
        }
    } else if (cause_code == E_BP) {
        DEBUG_LOG("Exception recognised as breakpoint");
        if (is_passup_set(SYS, interrupted_proc)) { launch_spec_area(SYS, interrupted_state); }
        else { adderrbuf("ERROR: BreakPoint launched, handler still not implemented!"); }
    }
    flush_kernel_time(interrupted_proc);
    conclude_handler(SYS);
}


void handle_TLB() {
    DEBUG_LOG("HANDLING TLB EXCEPTION");
    start_handler();
    if (is_passup_set(TLB, get_running_proc())) {
        DEBUG_LOG("Spec areas set, launching custom handler");
        launch_spec_area(TLB, GET_AREA(OLD, TLB));
    }

    terminate_proc(get_running_proc());
    conclude_handler(TLB);
}


void handle_trap() {
    DEBUG_LOG("HANDLING PROGRAM TRAP EXCEPTION");
    start_handler();
    if (is_passup_set(PRG, get_running_proc())) {
        DEBUG_LOG("Spec areas set, launching custom handler");
        launch_spec_area(PRG, GET_AREA(OLD, PRG));
    }

    terminate_proc(get_running_proc());
    conclude_handler(PRG);
}
