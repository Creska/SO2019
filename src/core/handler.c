#include <core/handler.h>


void handle_interrupt() {
    state_t* interrupted_process = get_old_area_int();

    consume_interrupts();
    LDST(interrupted_process);                                 // Resume the execution of the process

    // TODO check that the process is resumed with the pc at the right address (there could be an offset of a few instructions)
}

void handle_sysbreak() {

}
