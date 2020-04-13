#include "testing/our_tests.h"
#include "core/system/system.h"
#include "utils/debug.h"
#include "bikaya.h"
#include "core/processes/scheduler.h"

void loop_test() {
    addokbuf("Sono il test interno\n");

    DEBUG_LOG_INT("VM on:", get_virtual_mem(&get_running_proc()->p_s));

    SYSCALL(3,0,0,0);
}

void loop_test2() {
    while (1) {
        DEBUG_LOG("askfdhsaldjasld");
    }
}


void launcher() {


    DEBUG_LOG_INT("VM on:", get_virtual_mem(&get_running_proc()->p_s));

    addokbuf("dsafdsf\n");

    proc_init_data test_proc = {
            .method = loop_test,
            .priority = 0,
            .km_on = 1, .vm_on = 0,
            .timer_int_on = 1, .other_ints_on = 0
    };

    bikaya_add_proc(&test_proc);


    addokbuf("judsusjuu\n");
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
