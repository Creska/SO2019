#include "pcb.h"
#include "const.h"

/* PCB handling functions */


pcb_t pcbFree_table[MAXPROC];           // L'array in cui effettivamente risiedono i nostri PCB
struct list_head pcbFree_h;             // L'elemento sentinella della lista di PCB liberi


/* PCB free list handling functions */
void initPcbs(void) {
    INIT_LIST_HEAD(&pcbFree_h);

    for (int i = 0; i < MAXPROC; ++i) {
        freePcb(&pcbFree_table[i]);
    }
}

void freePcb(pcb_t *p) {
    // Ci appoggiamo al list_head p_next contenuto in ogni pcb_t, quando il pcb non è free questa credo serva a gestire la queue di processi, però
    // mentre il pcb è free ci possiamo appoggiare a questo list_head come a qualsiasi altro contenuto in pcb_t, tanto quando il PCB viene recuperato con allocPcb tutti i puntatori vengono inizializzati a NULL
    list_add(&p->p_next, &pcbFree_h);
}

pcb_t *allocPcb(void) {
    if (list_empty(&pcbFree_h)) {
        return NULL;
    } else {
        // Visto che la lista non è vuota recuperiamo un puntatore al list head successivo a quello della sentinella, ovvero il list head contenuto nel primo elemento della lista
        struct list_head* first_list_head = list_next(&pcbFree_h);
        list_del(first_list_head);      // Rimuoviamo il list_head dalla lista

        // Questa macro ci permette di recuperare il puntatore ad un oggetto che contiene un list head avendo solo il puntatore a quest'ultimo, il tipo che lo contiene e il nome del list head dentro alla struttura.
        // Noi ci appoggiamo a p_next per rappresentare la nostra lista di free pcb, quindi gli diamo lui
        struct pcb_t* first_free_pcb = container_of(first_list_head, struct pcb_t, p_next);

        // Inizializziamo tutti i campi a 0/NULL
        first_free_pcb->priority = 0;
        first_free_pcb->p_semkey = NULL;
        first_free_pcb->p_next.next = NULL;
        first_free_pcb->p_next.prev = NULL;
        first_free_pcb->p_child.next = NULL;
        first_free_pcb->p_child.prev = NULL;
        first_free_pcb->p_sib.next = NULL;
        first_free_pcb->p_sib.prev = NULL;
        first_free_pcb->p_parent = NULL;

        // first_free_pcb->p_s ...  TODO inizializzare questo campo a zero è un po' più difficile perchè cambia a seconda della piattaforma, probabilmente si tratta di fare una funzione statica in system che mette a 0 i campi diversi a seconda delle architetture, ma ce ne possiamo preoccupare in un secondo momento

        return first_free_pcb;
    }
}

/* PCB queue handling functions */
void mkEmptyProcQ(struct list_head *head) {

}
int emptyProcQ(struct list_head *head) {

}
void insertProcQ(struct list_head *head, pcb_t *p) {

}
pcb_t *headProcQ(struct list_head *head) {

}

pcb_t *removeProcQ(struct list_head *head) {

}
pcb_t *outProcQ(struct list_head *head, pcb_t *p) {

}


/* Tree view functions */
int emptyChild(pcb_t *this) {
    if (list_empty(&this->p_child)) {           // Non basta controllare che la lista sia vuota, perchè p_child potrebbe essere parte di una lista, ma essere l'ultimo elemento. In tal caso l'elemento che "segue" è quello in cima alla catena, dunque avrà parent diverso dal nostro target
        struct list_head* next_list_head = list_next(&this->p_child);
        struct pcb_t* next_pcb_in_list = container_of(next_list_head, struct pcb_t, p_child);
        if (next_pcb_in_list->p_parent!=this) return TRUE;
        else return FALSE;
    }
}

void insertChild(pcb_t *prnt, pcb_t *p) {

    if (p != NULL && prnt != NULL) {
        p->p_parent = prnt;                                       // p ha prnt come parent, entrambi i membri dell'espressione sono puntatori pcb_t
        if (emptyChild(prnt)) {
            list_add(&p->p_child, &prnt->p_child);
        } else {
            // Siccome il parent ha già un figlio aggiungiamo il nuovo pcb alla struttura come sibling nella lista in cui il primo figlio funge da sentinella
            struct list_head* sib_first_child = list_next(&prnt->p_child);
            struct pcb_t* first_child_pcb = container_of(sib_first_child, struct pcb_t, p_child);
            list_add(&p->p_sib, &first_child_pcb->p_sib);
        }
    } else {
        // TODO error message
    }
}

pcb_t *removeChild(pcb_t *p) {


}

pcb_t *outChild(pcb_t *p) {

}


