#include "testing/p2test_bikaya_v0.1.h"
#include "bikaya.h"

int main() {

//     Array containing all the relevant data for process initialization
    proc_init_data starting_processes[] =  {{
        .method = test,
        .priority = 1,
        .km_on = 1, .vm_on = 0,
        .timer_int_on = 1, .other_ints_on = 1 }};

    // Bikaya settings
    bikaya_init_data init_data = {
            .time_slice = 3000,
            .starting_procs_data = starting_processes,
            .starting_procs_n = 1
    };

    bikaya_launch(&init_data);
}

