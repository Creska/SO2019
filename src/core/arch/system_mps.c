#include <devices/terminal.h>
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


void set_interrupt_mask(state_t* s, unsigned int mask) {
    s->status = (~STATUS_IM_MASK & s->status) | (mask << STATUS_INT_MASK_BIT) | 1;          // TEMP the last |1
}



void set_kernel_mode(state_t *s, unsigned int on) {
    s->status = set_bits(s->status, STATUS_KUc, on << STATUS_KUc_BIT);
}

void set_virtual_mem(state_t *s, unsigned int on)
{
    s->status = set_bits(s->status, STATUS_VMc, on << STATUS_VMc_BIT);
}




