#include <utils/utils.h>
#include "core/system.h"

void reset_state(state_t *s)
{
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

//state_t della new_area
void set_pc(state_t * s, void (*ptr)()) { //ptr puntatore all'handler assumendolo con la signature placeholder void handler()
s->pc = (unsigned int)ptr;
}

void set_sp(state_t *s, unsigned int sp_val)
{
    s->sp = sp_val; //RAM_TOP is defined in arch.h with the value needed
}

void set_virtual_mem(state_t *s, unsigned int on)
{
    if (on) {
        s->CP15_Control = CP15_ENABLE_VM(s->CP15_Control);
    } else {
        s->CP15_Control = CP15_DISABLE_VM(s->CP15_Control);
    }
}

void set_kernel_mode(state_t *s, unsigned int on)
{
    if (on) {
        s->cpsr = set_bits(s->cpsr, ~STATUS_CLEAR_MODE, STATUS_SYS_MODE);
    } else {
        s->cpsr = set_bits(s->cpsr, ~STATUS_CLEAR_MODE, STATUS_USER_MODE);
    }
}