#include <utils/debug.h>
#include "bikaya.h"
#include "core/scheduler.h"
#include "core/handler.h"



void bikaya_initialize() {

    LOG("Initialization");

    // New areas initialization
    state_t* int_new_area = get_new_area_int();
    init_area(int_new_area, handle_interrupt);

    state_t* sysbrk_new_area = get_new_area_sys_break();
    init_area(sysbrk_new_area, handle_sysbreak);

    state_t* TLB_new_area = get_new_area_TLB();
    init_area(TLB_new_area, handle_TLB);

    state_t* trap_new_area = get_new_area_program_trap();
    init_area(trap_new_area, handle_trap);

    // Scheduler initialization
    init_scheduler();
}


void bikaya_quick_launch(void *method, unsigned int priority, unsigned int vm_on, unsigned int km_on,
                         unsigned int int_timer_on, unsigned int other_int_on) {
    bikaya_initialize();
    add_process(method, priority, vm_on, km_on, int_timer_on, other_int_on);
    launch();
}

void bikaya_launch(struct process_init_data *starting_processes, unsigned int len) {
    bikaya_initialize();
    for (int i = 0; i < len; ++i) {
    }
}
