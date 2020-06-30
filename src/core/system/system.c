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

void load_syscall_registers(state_t* s, unsigned int* n, unsigned int* a1, unsigned int* a2, unsigned int* a3) {
#ifdef TARGET_UMPS                                  // (handled with ifdef for now to avoid useless complexity, in the next phase this kind of stuff could be handled with a pattern similar to system.h)
    *n = s->reg_a0;
    *a1 = s->reg_a1;
    *a2 = s->reg_a2;
    *a3 = s->reg_a3;
#elif TARGET_UARM
    *n = s->a1;
    *a1 = s->a2;
    *a2 = s->a3;
    *a3 = s->a4;
#endif
}

unsigned int load_syscall_arg1(state_t* s) {
#ifdef TARGET_UMPS                                  // (handled with ifdef for now to avoid useless complexity, in the next phase this kind of stuff could be handled with a pattern similar to system.h)
    return s->reg_a1;
#elif TARGET_UARM
    return s->a2;
#endif
}


void save_syscall_return_register(state_t *s, unsigned int return_val) {
#ifdef TARGET_UMPS
    s->reg_v0 = return_val;
#elif TARGET_UARM
    s->a1 = return_val;
#endif
}



