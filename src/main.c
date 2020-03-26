#include "devices/terminal.h"
#include "core/system.h"

int main() {

    // Inizializzazione new areas
    state_t* interrupt = get_new_area_int();
    char buf[256];
    itoa(interrupt, buf);
    term_putstr(get_terminal(0), buf);
}

