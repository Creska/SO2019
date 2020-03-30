#include "devices/terminal.h"
#include "core/system.h"
#include "core/handler.h"
#include "core/scheduler.h"

int main() {

    DEBUG_LOG("Modules initialization");
    init_scheduler();



    // Testing get_areas and pointer printing functionality

    DEBUG_LOG("Inizializzazione new areas");

    state_t* int_new_area = get_new_area_int();
    init_area(int_new_area, 255, &handle_interrupt);

    DEBUG_LOG_PTR("Interrupt new area address: ", int_new_area);


    while (1) {
        DEBUG_LOG("yo");
    }

}

