#include <core/handler.h>


void handle_interrupt() {
    consume_interrupts();
    LDST(&get_running_proc()->p_s);                                 // Resume the execution of the process
}

void handle_sysbreak() {

    unsigned int cause_code = get_exccode(get_old_area_sys_break());

    state_t* s = get_old_area_sys_break();

    if (cause_code == EXCODE_SYS) {
        DEBUG_LOG("SYS");


        unsigned int a0;
#ifdef TARGET_UMPS

        s->pc_epc += WORD_SIZE;

        a0 = s->reg_a0;

        DEBUG_LOG_INT("a0: ", s->reg_a0);
        DEBUG_LOG_INT("a1: ", s->reg_a1);
        DEBUG_LOG_INT("a2: ", s->reg_a2);
        DEBUG_LOG_INT("a3: ", s->reg_a3);

#elif TARGET_UARM

        a0 = s->a1;

        DEBUG_LOG_INT("a0: ", s->a1);
        DEBUG_LOG_INT("a1: ", s->a2);
        DEBUG_LOG_INT("a2: ", s->a3);
        DEBUG_LOG_INT("a3: ", s->a4);

#endif


        switch (a0) {
            case 3:
                syscall3();
                break;
            default:
                adderrbuf("ERROR: Syscall not recognised");
        }


    } else if (cause_code == EXCODE_BP) {
        DEBUG_LOG("BP");
    }

    LDST(s);

}
