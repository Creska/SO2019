#include <core/processes/scheduler.h>
#include "testing/our_tests.h"
#include "core/system/system.h"
#include "utils/debug.h"

void loop_test() {
    DEBUG_LOG("ciao");
    SYSCALL(3,0,0,0);
}

void loop_test2() {
    while (1) {
        DEBUG_LOG("askfdhsaldjasld");
    }
}

void empty_loop() {
    SYSCALL(1,42,11,6);

    DEBUG_LOG("asfdasfdafs");

    SYSCALL(3,41,11,6);
}

void launcher() {

    add_process(loop_test, 4, 0, 1, 1, 0);


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
