#include <testing/our_tests.h>
#include "devices/terminal.h"
#include "core/system.h"
#include "core/handler.h"
#include "core/scheduler.h"

int main() {


    // Testing get_areas and pointer printing functionality

    // Initialize interrupt new area
    state_t* int_new_area = get_new_area_int();
    init_area(int_new_area, handle_interrupt);

    // Initialize syscall/breakpoint new area
    //state_t* sysbrk_new_area = get_new_area_sys_break();
    //init_area(sysbrk_new_area, 0, handle_sysbreak);


    // TEMP enabling interrupts on the coprocessor (this will go in its own cp initialization function)
    #ifdef TARGET_UARM
        setSTATUS(getSTATUS() & ~(1<<6));
    #elif TARGET_UMPS
        setSTATUS(getSTATUS()| (1<<10) | 1);
    #endif


    init_scheduler();
    set_interval_timer(get_ticks_per_slice());


    add_process(loop_test, 1, 0, 1, 1);

    run();



}

