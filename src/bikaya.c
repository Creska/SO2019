#include "bikaya.h"
#include "utils/debug.h"
#include "core/processes/scheduler.h"
#include "core/exceptions/handler.h"
#include "core/exceptions/syscalls.h"


// Initialization routine for the relevant OS' structures
void initialize_new_areas() {

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


void bikaya_launch(bikaya_init_data* init_data) {
    DEBUG_LOG("Debug logs are enabled, set CMAKE_BUILD_TYPE to any other value than Debug to disable them");
    DEBUG_SPACING;
    DEBUG_LOG("BIKAYA INITIALIZATION");
    initialize_new_areas();

    DEBUG_LOG("Scheduler initialization");
    init_scheduler(init_data->starting_procs_data, init_data->starting_procs_n, init_data->time_slice);

    DEBUG_LOG("Initialization completed, launching the system");

    launch();
}

void bikaya_add_proc(proc_init_data *new_proc) {
    SYSCALL(SYSCALL_ADD_PROC, (unsigned int)new_proc,0,0);
}

