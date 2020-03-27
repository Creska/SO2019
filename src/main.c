#include "devices/terminal.h"
#include "core/system.h"

int main() {

    // Testing get_areas and pointer printing functionality

    DEBUG_LOG("Inizializzazione new areas");

    state_t* interrupt = get_new_area_TLB();

    DEBUG_LOG_PTR("Interrupt new area address: ", interrupt);
}

