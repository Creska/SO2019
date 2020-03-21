#include "core/system.h"

void reset_state(state_t* s) {
    s->a1 = 0;
    s->a2 = 0;
    s->a3 = 0;
    s->a4 = 0;
    s->CP15_Cause = 0;
    s->CP15_Control = 0;
    s->cpsr = 0;
    s->TOD_Hi = 0;
    s->TOD_Low = 0;
    s->fp = 0;
    s->lr = 0;
    s->pc = 0;
    s->sp = 0;
    s->sl = 0;
    s->ip = 0;
    s->v1 = 0;
    s->v2 = 0;
    s->v3 = 0;
    s->v4 = 0;
    s->v5 = 0;
    s->v6 = 0;
    s->CP15_EntryHi = 0;
}

void set_sp(state_t *s, unsigned int sp_val) {
    s->sp = sp_val;
}

void set_kernel_mode(state_t *s, unsigned int on) {

}




