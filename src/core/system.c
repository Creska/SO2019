#include "core/system.h"



void resetState(state_t* s) {

#ifdef TARGET_UMPS

    s->pc_epc = 0;
    s->hi = 0;
    s->entry_hi = 0;
    s->cause = 0;
    s->status = 0;
    s->lo = 0;
    for (int i = 0; i < STATE_GPR_LEN; ++i) {
        s->gpr[i] = 0;
    }

#endif

#ifdef TARGET_UARM

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

#endif

}

