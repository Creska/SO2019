#include <devices/terminal.h>
#include "testing/our_tests.h"
#include "core/system.h"

void loop_test() {
    while (1) {
        DEBUG_LOG("yoooooooooo");
    }
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
