#include "core/system.h"


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
s->reg_t9 = (unsigned int)ptr;                  // TODO controllare che gli indici corrispondano (stranezze regdef.h)
}


void set_sp(state_t *s, unsigned int sp_val) {
   s->reg_sp = sp_val;
}


void set_interrupt_mask(state_t* s, unsigned int mask) {
    unsigned int bit_mask = 0xFF00;                             // Mask where only the bits corresponding to the interrupt masks are 1 TODO declare it as a umps system macro
    s->status = (~bit_mask & s->status) | mask << 8;
}



