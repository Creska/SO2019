#include <devices/terminal.h>
#include <core/scheduler.h>
#include "testing/our_tests.h"
#include "core/system.h"

void loop_test() {
    DEBUG_LOG("ciao");
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
    LOG("Starting launcher process");

    //add_process(loop_test, 4, 0, 1, 1);

    LOG("Ending launcher");
}
