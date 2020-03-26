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

