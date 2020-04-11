#include "core/handler.h"
#include "utils/debug.h"


void handle_interrupt() {
    consume_interrupts();
    LDST(&get_running_proc()->p_s);                                 // Resume the execution of the process
}

void handle_sysbreak() {

    LOG("SYS");

    unsigned int cause_code = get_exccode(get_old_area_sys_break());

    state_t* s = get_old_area_sys_break();

    if (cause_code == EXCODE_SYS) {

        unsigned int a0, a1, a2, a3;                // Retrieving syscall type and arguments from processor registers
#ifdef TARGET_UMPS                                  // (handled with ifdef for now to avoid useless complexity, in the next phase this kind of stuff could be handled with a pattern similar to system)
        s->pc_epc += WORD_SIZE;

        a0 = s->reg_a0;
        a1 = s->reg_a1;
        a2 = s->reg_a2;
        a3 = s->reg_a3;
#elif TARGET_UARM
        a0 = s->a1;
        a1 = s->a2;
        a2 = s->a3;
        a3 = s->a4;
#endif

        switch (a0) {
            case 3:
                syscall3();
                break;
            default:
                adderrbuf("ERROR: Syscall not recognised");
        }

    } else if (cause_code == EXCODE_BP) {
        adderrbuf("ERROR: BreakPoint launched, handler still not implemented!");
    }

    LDST(s);
}

void handle_TLB() {
    adderrbuf("ERROR: TLB exception handling not implemented yet");
}

void handle_trap() {
    adderrbuf("ERROR: program trap handling not implemented yet");
}
