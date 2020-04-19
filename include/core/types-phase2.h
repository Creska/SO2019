#ifndef BIKAYA_PHASE0_TYPES_PHASE2_H
#define BIKAYA_PHASE0_TYPES_PHASE2_H

#ifndef TYPES_BIKAYA_H_INCLUDED
#define TYPES_BIKAYA_H_INCLUDED

#include "core/system/system.h"
#include "core/processes/asl.h"

#include "const.h"
#include "core/types-phase2.h"
#include "utils/listx.h"

typedef unsigned int memaddr;



typedef struct semdev {
    semd_t disk[DEV_PER_INT];
    semd_t tape[DEV_PER_INT];
    semd_t network[DEV_PER_INT];
    semd_t printer[DEV_PER_INT];
    semd_t terminalR[DEV_PER_INT];
    semd_t terminalT[DEV_PER_INT];
} semdev;

#endif



#endif //BIKAYA_PHASE0_TYPES_PHASE2_H
