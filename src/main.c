#include <testing/our_tests.h>
#include <utils/debug.h>
#include "testing/p1.5test_bikaya_v0.h"
#include "core/processes/scheduler.h"
#include "bikaya.h"

int main() {

    bikaya_initialize();

    add_process(test1, 1, 0, 1, 1, 0);
    add_process(test2, 2, 0, 1, 1, 0);
    add_process(test3, 3, 0, 1, 1, 0);
//     add_process(debug_log_tester, 3, 0, 1, 1, 0);


    launch();



}

