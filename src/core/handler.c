#include <core/handler.h>
#include "regdef.h"


void handle_interrupt() {

    consume_interrupts();
    LDST(&get_running_proc()->p_s);                                 // Resume the execution of the process
}

void handle_sysbreak() {

    unsigned int cause_code = get_exccode(getCAUSE());

    state_t* s = get_old_area_sys_break();

    if (cause_code == EXCODE_SYS) {
        DEBUG_LOG("SYS");

        if (s->reg_a0==3) {

        }

#ifdef TARGET_UMPS

        s->pc_epc += WORD_SIZE;


        DEBUG_LOG_INT("a0: ", s->reg_a0);
        DEBUG_LOG_INT("a1: ", s->reg_a1);
        DEBUG_LOG_INT("a2: ", s->reg_a2);
        DEBUG_LOG_INT("a3: ", s->reg_a3);

#elif TARGET_UARM

        DEBUG_LOG_INT("a0: ", s->reg_a0);
        DEBUG_LOG_INT("a1: ", s->reg_a1);
        DEBUG_LOG_INT("a2: ", s->gpr[a2]);
        DEBUG_LOG_INT("a3: ", s->gpr[a3]);

#endif

    } else if (cause_code == EXCODE_BP) {
        DEBUG_LOG("BP");
    }

    LDST(s);



}
