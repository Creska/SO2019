#ifndef PCB_H
#define PCB_H

#include "core/system/system.h"
#include "utils/listx.h"
#include "core/const.h"
#include "devices/terminal.h"


typedef struct pcb_t {
    /*process queue fields */
    struct list_head p_next;

    /*process tree fields */
    struct pcb_t *   p_parent;
    struct list_head p_child, p_sib;

    /* processor state, etc */
    state_t p_s;

    /* process priority, increased while the process stays in the ready queue */
    int priority;

    /* original priority of the process at schedule */
    int original_priority;                                          // this was added for phase 1.5

    /* key of the semaphore on which the process is eventually blocked */
    int *p_semkey;

    /* The device waiting list this process is enqueued on (NULL if not enqueued) */
    struct pcb_t **dev_w_list;


    unsigned int kernel_timer;
    unsigned int user_timer;

    // Holds the value of the TOD at the last "mode switch"
    unsigned int tod_cache;

    // Holds the value of the TOD at the first execution of the process
    unsigned int tod_at_start;

    // Array of pointers to the old/new areas defined through the SPECPASSUP syscall
    // Order: sys_old | sys_new | TLB_old | TLB_new | trap_old | trap_new
    state_t* spec_areas[6];

} pcb_t;


// PCB hHANDLING FUNCTIONS --------------------------------------------------------------------------------------------

//  Inizializza la pcbFree in modo da contenere tutti gli elementi della pcbFree_table.
//  Questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati.
void initPcbs(void);

//  Inserisce il PCB puntato da p nella lista dei PCB liberi.
void freePcb(pcb_t *p);

// // Restituisce NULL se la pcbFree è vuota. Altrimenti rimuove un elemento dalla lista di pcb free e l restituisce con tutti i campi inizializzati a 0/NULL
pcb_t *allocPcb(void);



// PCB QUEUE HANDLING FUNCTIONS ---------------------------------------------------------------------------------------

// Inizializza la lista dei PCB, inizializzando l’elemento sentinella
void mkEmptyProcQ(struct list_head *head);

// Restituisce TRUE se la lista puntata da head è vuota, FALSE altrimenti
int emptyProcQ(struct list_head *head);

// Inserisce l’elemento puntato da p nella coda dei processi puntata da head
// L’inserimento avviene tenendo conto della priorita’ di ciascun pcb, mantenendo la lista in ordine decrescente
void insertProcQ(struct list_head *head, pcb_t *p);

// Restituisce l’elemento di testa della coda dei processi da head, SENZA RIMUOVERLO. Ritorna NULL se la coda non ha elementi
pcb_t *headProcQ(struct list_head *head);

// Rimuove il primo elemento dalla coda dei processi puntata da head. Ritorna NULL se la coda è vuota. Altrimenti ritorna il puntatore all’elemento rimosso dalla lista
pcb_t *removeProcQ(struct list_head *head);

// Rimuove il PCB puntato da p dalla coda dei processi puntata da head. Se p non è presente nella coda, restituisce NULL
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

// PCB TREE FUNCTIONS -------------------------------------------------------------------------------------------------

// Restituisce TRUE se il PCB puntato da p non ha figli, restituisce FALSE altrimenti.
int emptyChild(pcb_t *this);

// Inserisce il PCB puntato da p come figlio del PCB puntato da prnt
void insertChild(pcb_t *prnt, pcb_t *p);

// Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL
pcb_t *removeChild(pcb_t *p);

// Rimuove il PCB puntato da p dalla lista dei figli del padre.
// Se il PCB puntato da p non ha un padre, restituisce NULL
pcb_t *outChild(pcb_t *p);

// Restituisce il prossimo fratello nella lista di fratelli, NULL se quello dato è l'ultimo della lista.
pcb_t *nextSibling(pcb_t *target_sibling, pcb_t* first_sibling);

// Returns the index that the given pcb occupies in the internal array, it can double as a unique process id
unsigned int get_pcb_index(pcb_t* p);

#endif
