// System source file containing definitions for functions with the same logic for both architectures

#include "core/system/system.h"


void set_interval_timer(unsigned int val) {
    *(unsigned int*)BUS_REG_TIMER = val;
}

void init_new_area(state_t *area, void (*handler)()) {
    set_kernel_mode(area, 1);
    set_virtual_mem(area, 0);
    set_interval_timer_interrupts(area, 0);
    set_other_interrupts(area, 0);
    set_sp(area, RAM_TOP);
    set_pc(area, handler);
}

unsigned int clock_ticks_per_period(unsigned int microseconds) {
    return *(unsigned int*)BUS_REG_TIME_SCALE * microseconds;
}