#include "core/exceptions/syscalls.h"


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

void save_return_register(state_t *s, unsigned int return_val) {
#ifdef TARGET_UMPS
    s->reg_v0 = return_val;
#elif TARGET_UARM
    s->a1 = return_val;
#endif
}

