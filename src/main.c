#include "devices/terminal.h"
#include "core/system.h"

int main() {

    // Inizializzazione new areas
    state_t* interrupt = get_new_area_TLB();            // Testing get_areas and pointer printing functionality
    char buf[256];
    ptr_to_str(interrupt, buf);
    term_putstr(get_terminal(0), buf);
}

