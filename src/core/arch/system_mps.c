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


void set_sp(state_t *s, unsigned int sp_val) {
    s->gpr[28] = sp_val;                             // TODO that index needs to be defined by a macro
}

void set_kernel_mode(state_t *s, unsigned int on) {
    s->status ^= (-on ^ s->status) & (1UL << STATUS_KUc_BIT);

//    s->status = set_bit_at(s->status, on, STATUS_KUc_BIT);
}

