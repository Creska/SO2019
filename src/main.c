#include <devices/terminal.h>
#include "core/system.h"
#include "core/handler.h"

int main() {

    // Testing get_areas and pointer printing functionality

    DEBUG_LOG("Inizializzazione new areas");

    state_t* int_new_area = get_new_area_int();
    init_area(int_new_area, 255, &handle_interrupt);

#define TIME_SCALE *((unsigned int *)BUS_REG_TIME_SCALE)

    DEBUG_LOG_INT("time scale", TIME_SCALE);

    DEBUG_LOG_PTR("Interrupt new area address: ", int_new_area);

//    DEBUG_LOG_BININT("Control register before set: ", int_new_area->CP15_Control);
//    set_virtual_mem(int_new_area, 0);
//    DEBUG_LOG_BININT("Control register after set: ", int_new_area->CP15_Control);




//    set_interval_timer(1000000);
//    while (1) {
//
//    }


}

