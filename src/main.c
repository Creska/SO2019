#include "devices/terminal.h"
#include "core/system.h"

int main() {

    // Inizializzazione new areas

    state_t* interrupt = get_new_area_interrupt();
    set_kernel_mode(interrupt, 1);
    set_virtual_mem(interrupt, 0);
    // set_interrupt_mask(interrupt, ???)
}

