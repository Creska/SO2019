#include "bikaya.h"
#include "utils/debug.h"
#include "core/processes/scheduler.h"
#include "core/exceptions/handler.h"


// Initialization routine for the relevant OS' structures
void bikaya_initialize() {

    DEBUG_LOG("New areas initialization");
    state_t* int_new_area = get_new_area_int();
    init_new_area(int_new_area, handle_interrupt);

    state_t* sysbrk_new_area = get_new_area_sys_break();
    init_new_area(sysbrk_new_area, handle_sysbreak);

    state_t* TLB_new_area = get_new_area_TLB();
    init_new_area(TLB_new_area, handle_TLB);

    state_t* trap_new_area = get_new_area_program_trap();
    init_new_area(trap_new_area, handle_trap);
}


void bikaya_launch(proc_init_data starting_procs[], unsigned int procs_number) {
    DEBUG_LOG("BIKAYA INITIALIZATION");
    bikaya_initialize();

    DEBUG_LOG("Scheduler initialization");
    init_scheduler(starting_procs, procs_number);

    DEBUG_SPACING;

    launch();
}

void bikaya_add_proc(proc_init_data *new_proc) {
    SYSCALL(20, (unsigned int)new_proc,0,0);
}

