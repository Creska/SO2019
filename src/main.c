#include "devices/terminal.h"
#include "core/system.h"
#include "core/handler.h"
#include "core/scheduler.h"

int main() {


    // Testing get_areas and pointer printing functionality

    //DEBUG_LOG("Inizializzazione new areas");

    state_t* int_new_area = get_new_area_int();
    init_area(int_new_area, 0, handle_interrupt);


    // TEMP enabling interrupts on the coprocessor (this will go in is own cp initialization function)
    #ifdef TARGET_UARM
        setSTATUS(getSTATUS() & ~(1<<6));
    #elif TARGET_UMPS
        setSTATUS(getSTATUS()| (1<<10) | 1);
    #endif


    init_scheduler();


    set_interval_timer(get_ticks_per_slice());


    while (1) {
        DEBUG_LOG("yo");
    }
}

