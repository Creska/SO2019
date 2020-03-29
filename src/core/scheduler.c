#include "core/scheduler.h"


// interrupt
//      - controlla priorità primo processo nella ready queue
//      - se è superiore a quella del processo in esecuzione metti il nuovo processo in esecuzione e rimetti il vecchio processo nella ready queue (ripristinando la priorità a quella originale)
//      - aumenta la priority di tutti i processi nella ready queue in modo da evitare starvation
//      - continua l'esecuzione del processo


void check() {

}
