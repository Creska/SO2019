#include "core/system.h"
#include "utils/utils.h"


void reset_state(state_t* s) {
    s->pc_epc = 0;
    s->hi = 0;
    s->entry_hi = 0;
    s->cause = 0;
    s->status = 0;
    s->lo = 0;
    for (int i = 0; i < STATE_GPR_LEN; ++i) {
        s->gpr[i] = 0;
    }
}


//state_t della new_area
void set_pc(state_t * s, void (*ptr)()) {       //ptr puntatore all'handler assumendolo con la signature void handler()
s->pc_epc = (unsigned int)ptr;
s->reg_t9 = (unsigned int)ptr;
}


void set_sp(state_t *s, unsigned int sp_val) {
   s->reg_sp = sp_val;
}


void set_kernel_mode(state_t *s, unsigned int on) {
    if (on) {
        s->status = s->status & (~STATUS_KUc);                                            // the kernel mode is on when the bit is 0
    } else {
        s->status = s->status | (~STATUS_KUc);
    }
}

void set_virtual_mem(state_t *s, unsigned int on)
{
    s->status = set_bits(s->status, STATUS_VMc, on << STATUS_VMc_BIT);
}

unsigned int is_interrupt_pending(unsigned int line) {
    return (getCAUSE() & CAUSE_IP(line)) >> CAUSE_IP_BIT(line);
}

void set_interrupts(state_t *s, unsigned int on) {
    if (on) {
        s->status = s->status | STATUS_IM_MASK | 1;         // Turn on all bits of the interrupt mask (plus the global bit)
    } else {
        s->status = s->status & ~STATUS_IM_MASK & ~1;       // Turn off all bits
    }
}


unsigned int get_exccode(state_t* state) {
    unsigned int val = CAUSE_GET_EXCCODE(state->cause);

    switch (val) {
        case EXC_BP:
            return EXCODE_BP;

        case EXC_SYS:
            return EXCODE_SYS;

        default:
            return EXCODE_OTHER;
    }
}





