#include "core/exceptions/handler.h"
#include "utils/utils.h"


// During handler execution (after start_handler()) this pointer is set to the PCB that was running while the exception was raised
pcb_t* interrupted_proc;

// Cache for the interval timer set at the beginning of an handler and used to restore the interval
// timer value at the end of the handler (when appropriate)
unsigned int int_timer_cache;



// Passup areas utility functions -------------------------------------------------------------------------------------

// Returns a pointer to the passup area of a given process
state_t *get_passup_area(enum area_age area_age, enum exc_type area_type, pcb_t* proc);

// Returns 1 if the passup areas (old AND new) are set for a given process and for a given exception type.
int is_passup_set(enum exc_type area_type, pcb_t* p);

// Executes the passup area of the running process of the given exception type.
void launch_spec_area(int exc_type, state_t* interrupted_state);


// Handler routines ---------------------------------------------------------------------------------------------------

// Routine called at the beginning of each handler. Manages interrupted process and interval timer caching, user time flushing etc.
void start_handler();

// Routine called at the end of each handler. Manages kernel time flushing, interval timer restoring/resetting and resumes the running process.
void conclude_handler(enum exc_type exc_type);



// HANDLER FUNCTIONS ==================================================================================================
// Each handler function is guaranteed to be called when the corresponding exception is raised. These functions have
// a common structure: start_handler() -> <handler logic> -> conclude_handler()
// The <handler logic> can cause a change of running process, for example a syscall could cause the scheduling of a
// new process with high priority, or a program trap exception could terminate the currently running process.
// It's conclude_handler's job to resume normal execution in the most sensible way. For example if the running process
// has changed the old running process' state is copied back into the pcb's state from the old area in order to avoid
// information loss.
// All handlers except handle_interrupt can also pass execution to user defined handlers (if defined and if appropriate),
// so it should be noted that conclude_handler() is not necessarily reached every time.


void handle_interrupt() {
    DEBUG_LOG_UINT("HANDLING INTERRUPT EXCEPTIONS during proc ", get_proc_scheduler_index(get_running_proc()));
    start_handler();

    consume_interrupts();           // Actually handle the interrupts pending

    conclude_handler(INT);
}


void handle_sysbreak() {

    DEBUG_LOG_UINT("HANDLING SYSCALL/BREAKPOINT EXCEPTION during proc ", get_proc_scheduler_index(interrupted_proc));
    start_handler();

    state_t *interrupted_state = AREA(OLD, SYS);
#ifdef TARGET_UMPS
    interrupted_state->pc_epc += WORD_SIZE;
#endif

    enum exc_code cause_code = get_exccode(interrupted_state);      // Retrieve a platform-independent exception cause code
    if (cause_code == E_SYS) {
        if (SYSCALL_N(interrupted_state) > SYSTEM_SYSCALL_MAX) {
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
    } else { adderrbuf("Exc cause code not recognised"); }

    conclude_handler(SYS);
}


void handle_TLB() {
    DEBUG_LOG("HANDLING TLB EXCEPTION");
    start_handler();
    if (is_passup_set(TLB, get_running_proc())) {
        DEBUG_LOG("Spec areas set, launching custom handler");
        launch_spec_area(TLB, AREA(OLD, TLB));
    } else {
        DEBUG_LOG("WARNING: a TLB exception was raised, terminating the process");
        terminate_proc(get_running_proc());
        conclude_handler(TLB);
    }
}


void handle_trap() {
    DEBUG_LOG("HANDLING PROGRAM TRAP EXCEPTION");
    start_handler();
    if (is_passup_set(PRG, get_running_proc())) {
        DEBUG_LOG("Spec areas set, launching custom handler");
        launch_spec_area(PRG, AREA(OLD, PRG));
    } else {
        DEBUG_LOG("WARNING: a program trap was raised, terminating the process");
        terminate_proc(get_running_proc());
        conclude_handler(PRG);
    }
}


// PRIVATE FUNCTIONS ==================================================================================================

state_t *get_passup_area(enum area_age area_age, enum exc_type area_type, pcb_t* proc) {
    return proc->spec_areas[area_type*2 + area_age];
}

int is_passup_set(enum exc_type area_type, pcb_t* p) {
    state_t* old_area = p->spec_areas[area_type*2];
    state_t* new_area = p->spec_areas[area_type*2+1];
    return ((old_area)!=NULL && (new_area)!=NULL);
}

void launch_spec_area(int exc_type, state_t* interrupted_state) {
    state_t* spec_old_area = get_passup_area(OLD, exc_type, get_running_proc());
    memcpy(spec_old_area, interrupted_state, sizeof(state_t));
    state_t* spec_new_area = get_passup_area(NEW, exc_type, get_running_proc());
    LDST(spec_new_area);
}

void start_handler() {
    int_timer_cache = INTERVAL_TIMER;
    if (int_timer_cache > get_clock_ticks_per_time_slice()) {       // The interval timer under-flowed, this avoids
        int_timer_cache = get_clock_ticks_per_time_slice();         // restoring an incorrect value at the end of the handler
    }
    interrupted_proc = get_running_proc();
    flush_user_time(interrupted_proc);
}

void conclude_handler(enum exc_type exc_type) {

#ifdef DEBUG
    debug_ready_queue();
    debug_asl();
#endif

    pcb_t* resuming_proc = get_running_proc();
    if (interrupted_proc != resuming_proc) {
        flush_reset_time(resuming_proc);                    // Don't count the kernel time if we're swapping processes
        reset_int_timer();                                  // give the new running process a full time slice
        if (interrupted_proc!=get_idle_proc())
            memcpy(&interrupted_proc->p_s, AREA(OLD, exc_type), sizeof(state_t));
        DEBUG_SPACING;
        LDST(&resuming_proc->p_s);
    } else {
        INTERVAL_TIMER = int_timer_cache;
        state_t* interrupted_state = AREA(OLD, exc_type);
        flush_kernel_time(interrupted_proc);
        DEBUG_SPACING;
        LDST(interrupted_state);                            // Resume the execution of the same process that was interrupted, retrieving it from the old area
    }
}
