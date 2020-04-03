#include <core/handler.h>


void handle_interrupt() {
    state_t* interrupted_process = get_old_area_int();

    consume_interrupts();


    LDST(&get_running_proc()->p_s);                                 // Resume the execution of the process

}

void handle_sysbreak() {

}
