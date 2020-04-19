#include "testing/our_tests.h"
#include "core/system/system.h"
#include "utils/debug.h"
#include "bikaya.h"


void launcher() {

    addokbuf("After this line i will schedule 19 processes\n");

    for (int i = 0; i < 19; ++i) {
        proc_init_data test_proc = {
                .method = dummy_process,
                .priority = i,
                .km_on = 1, .vm_on = 0,
                .timer_int_on = 1, .other_ints_on = 0
        };

        bikaya_add_proc(&test_proc);
    }

    addokbuf("I've terminate scheduling of the 19 processes");

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


void timer_tester() {
    while (1) {
        addokbuf("YOYOYOYOYOYO");
        unsigned int kernel, user, wallclock;
        SYSCALL(1, (unsigned int)&user, (unsigned int)&kernel, (unsigned int)&wallclock);

        DEBUG_LOG_UINT("User: ", user);
        DEBUG_LOG_UINT("Kernel: ", kernel);
        DEBUG_LOG_UINT("Wallclock: ", wallclock);
    }
}
