#include "core/system.h"
#include "utils/utils.h"
#define SP_INDEX  28
#define T_INDEX 26 //t9 register index assuming it is the 26th gpr which is uncertain
//the following three lines are copied from p1.5test_bikaya_v0.c
#define RAMBASE    *((unsigned int *)BUS_REG_RAM_BASE)//BUS_REG_RAM_BASE and BUS_REG_RAM_SIZE are defined in arch.h
#define RAMSIZE    *((unsigned int *)BUS_REG_RAM_SIZE)//as 0x10000000 and 0x10000004 respectively
#define RAMTOP     (RAMBASE + RAMSIZE)
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
void set_pc(state_t * s, void (*ptr)()) {//ptr puntatore all'handler assumendolo con la signature void handler()
s->pc_epc = (unsigned int)ptr;
s->gpr[T_INDEX] = (unsigned int)ptr;
}

void set_sp(state_t *s) {
   s->gpr[SP_INDEX] = RAMTOP;
}


//    s->status = set_bit_at(s->status, on, STATUS_KUc_BIT);
}
