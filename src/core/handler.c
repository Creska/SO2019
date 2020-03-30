#include <devices/terminal.h>
#include <core/system.h>
#include "core/scheduler.h"


void handle_interrupt() {
    time_slice_expired_callback();
}