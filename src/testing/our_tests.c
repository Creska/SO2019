#include "testing/our_tests.h"
#include "core/system/system.h"
#include "utils/debug.h"
#include "bikaya.h"
#include "core/processes/scheduler.h"


void launcher() {

    addokbuf("dsafdsf\n");

    for (int i = 0; i < 19; ++i) {
        proc_init_data test_proc = {
                .method = dummy_process,
                .priority = 1,
                .km_on = 1, .vm_on = 0,
                .timer_int_on = 1, .other_ints_on = 0
        };

        bikaya_add_proc(&test_proc);
    }

    SYSCALL(3, 0, 0, 0);
}

void debug_log_tester() {
    DEBUG_LOG("Test message 1");
    DEBUG_LOG("Test message 2");
    DEBUG_LOG("Test message 3");
    DEBUG_LOG("Test message 4");
    DEBUG_LOG("Test message 5");
    SYSCALL(3, 0, 0, 0);
}

void dummy_process() {
    addokbuf("I'm a dummy process, after writing this line i'll terminate myself\n");
    SYSCALL(3,0,0,0);
}
