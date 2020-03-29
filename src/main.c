#include "devices/terminal.h"
#include "core/system.h"
#include "core/handler.h"

int main() {

    // Testing get_areas and pointer printing functionality

    DEBUG_LOG("Inizializzazione new areas");

    state_t* int_new_area = get_new_area_int();
    init_area(int_new_area, 255, &handle_interrupt);

    DEBUG_LOG_PTR("Interrupt new area address: ", int_new_area);

    set_interval_timer(1000000);

}

