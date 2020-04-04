#include <devices/terminal.h>
#include "core/system.h"

void set_interval_timer(unsigned int val) {
    *(unsigned int*)BUS_REG_TIMER = val;
}

state_t *get_new_area_int() {
    return (state_t*)INT_NEWAREA;
}

state_t *get_new_area_TLB() {
    return (state_t*)TLB_NEWAREA;
}

state_t *get_new_area_program_trap() {
    return (state_t*)PGMTRAP_NEWAREA;
}

state_t *get_new_area_sys_break() {
    return (state_t*)SYSBK_NEWAREA;
}

state_t *get_old_area_sys_break() {
    return (state_t*)SYSBK_OLDAREA;
}

state_t *get_old_area_program_trap() {
    return (state_t*)PGMTRAP_OLDAREA;;
}

state_t* get_old_area_TLB() {
    return (state_t*)TLB_OLDAREA;
}

state_t* get_old_area_int() {
    return (state_t*)INT_OLDAREA;
}


void init_area(state_t *area, void (*handler)()) {
    set_kernel_mode(area, 1);
    set_virtual_mem(area, 0);
    set_interrupts(area, 0);
    set_sp(area, RAM_TOP);
    set_pc(area, handler);
}

unsigned int clock_ticks_per_period(unsigned int microseconds) {
    return *(unsigned int*)BUS_REG_TIME_SCALE * microseconds;
}


