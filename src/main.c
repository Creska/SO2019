#include "testing/p2test_bikaya_v0.1.h"
#include "testing/old/p1.5test_bikaya_v0.h"
#include "testing/our_tests.h"
#include "bikaya.h"

int main() {

    // Array containing all the relevant data for process initialization
//    proc_init_data starting_processes[] =  {
//
//            {.method = debug_log_tester,
//             .priority = 1,
//             .km_on = 1, .vm_on = 0,
//             .timer_int_on = 1, .other_ints_on = 0}
//    };



    proc_init_data starting_processes[] =  {

            {.method = test1,
                    .priority = 1,
                    .km_on = 1, .vm_on = 0,
                    .timer_int_on = 1, .other_ints_on = 0},
            {.method = test2,
                    .priority = 2,
                    .km_on = 1, .vm_on = 0,
                    .timer_int_on = 1, .other_ints_on = 0},
            {.method = test3,
                    .priority = 3,
                    .km_on = 1, .vm_on = 0,
                    .timer_int_on = 1, .other_ints_on = 0},

    };

    bikaya_init_data init_data = {
            .time_slice = 3000,
            .starting_procs_data = starting_processes,
            .starting_procs_n = 3
    };

    bikaya_launch(&init_data);

}

