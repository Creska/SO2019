#ifndef PCB_H
#define PCB_H
#include "types_bikaya.h"

/* PCB handling functions */

/* PCB free list handling functions */
void initPcbs(void);
void freePcb(pcb_t *p);
pcb_t *allocPcb(void);

/* PCB queue handling functions */

// Inizializza la lista dei PCB, inizializzando l’elemento sentinella
void mkEmptyProcQ(struct list_head *head);

// Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti
int emptyProcQ(struct list_head *head);

// Inserisce l’elemento puntato da p nella coda dei processi puntata da head
// L’inserimento avviene tenendo conto della priorita’ di ciascun pcb, mantenendo la lista in ordine decrescente
void insertProcQ(struct list_head *head, pcb_t *p);
pcb_t *headProcQ(struct list_head *head);

pcb_t *removeProcQ(struct list_head *head);
pcb_t *outProcQ(struct list_head *head, pcb_t *p);


/* Tree view functions */

// Restituisce TRUE se il PCB puntato da p non ha figli, restituisce FALSE altrimenti.
int emptyChild(pcb_t *this);

// Inserisce il PCB puntato da p come figlio del PCB puntato da prnt
void insertChild(pcb_t *prnt, pcb_t *p);

// Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL
pcb_t *removeChild(pcb_t *p);

// Rimuove il PCB puntato da p dalla lista dei figli del padre.
// Se il PCB puntato da p non ha un padre, restituisce NULL
pcb_t *outChild(pcb_t *p);

#endif
