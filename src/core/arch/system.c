#include "core/system.h"

void set_interval_timer(unsigned int val) {
    *(unsigned int*)BUS_REG_TIMER = val;
}

