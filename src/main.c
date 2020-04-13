#include <testing/our_tests.h>
#include "testing/p1.5test_bikaya_v0.h"
#include "bikaya.h"

int main() {

    // Array containing all the relevant data for process initialization
    proc_init_data data[] =  {

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
             .timer_int_on = 1, .other_ints_on = 0}};

    bikaya_launch(data, 3);
}

