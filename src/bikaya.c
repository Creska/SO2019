#include <utils/debug.h>
#include "bikaya.h"
#include "core/processes/scheduler.h"
#include "core/exceptions/handler.h"



void bikaya_initialize() {

    DEBUG_LOG("BIKAYA INITIALIZATION");

    DEBUG_LOG("New areas initialization");
    state_t* int_new_area = get_new_area_int();
    init_new_area(int_new_area, handle_interrupt);

    state_t* sysbrk_new_area = get_new_area_sys_break();
    init_new_area(sysbrk_new_area, handle_sysbreak);

    state_t* TLB_new_area = get_new_area_TLB();
    init_new_area(TLB_new_area, handle_TLB);

    state_t* trap_new_area = get_new_area_program_trap();
    init_new_area(trap_new_area, handle_trap);

    DEBUG_LOG("Scheduler initialization\n");
    init_scheduler();
}


void bikaya_quick_launch(void *method, unsigned int priority, unsigned int vm_on, unsigned int km_on,
                         unsigned int int_timer_on, unsigned int other_int_on) {
    bikaya_initialize();
    add_process(method, priority, vm_on, km_on, int_timer_on, other_int_on);
    launch();
}

