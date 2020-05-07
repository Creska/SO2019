#include "testing/our_tests.h"
#include "core/system/system.h"
#include "utils/debug.h"
#include "bikaya.h"
#include "arch.h"

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
    //line -> categoria di device
    //dev -> num. istanza del device
    unsigned int yo = DEV_REG_ADDR(3, 7);
    unsigned int dev_num = GET_DEV_INSTANCE(yo);
    unsigned int dev_line = GET_DEV_LINE(yo);
    //DEBUG_LOG_UINT("Device's number: ", dev_num);
    DEBUG_LOG_UINT("Device's instance: ", dev_num);
    DEBUG_LOG_UINT("Device's line: ", dev_line);
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

void proc_termination_tester() {
    addokbuf("I'm printing this message and then I'm going to terminate myself\n");
    SYSCALL(3, (unsigned int)NULL, 0, 0);
}

void proc_creation_tester() {
    // TODO test process creation
}
