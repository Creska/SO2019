#include <testing/our_tests.h>
#include "testing/p1.5test_bikaya_v0.h"
#include "core/system.h"
#include "core/handler.h"
#include "core/scheduler.h"

int main() {


    // Testing get_areas and pointer printing functionality

    // Initialize interrupt new area
    state_t* int_new_area = get_new_area_int();
    init_area(int_new_area, handle_interrupt);

    // Initialize syscall/breakpoint new area
     state_t* sysbrk_new_area = get_new_area_sys_break();
     init_area(sysbrk_new_area, handle_sysbreak);




    init_scheduler();


    add_process(empty_loop, 1, 0, 1, 1);
//    add_process(test2, 1, 0, 1, 1);
//    add_process(test3, 1, 0, 1, 1);

//    add_process(test2, 2, 0, 1, 1);
//    add_process(test3, 3, 0, 1, 1);


    launch();



}

