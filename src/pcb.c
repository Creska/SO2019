#include "pcb.h"


pcb_t pcbFree_table[MAXPROC];           // L'array in cui effettivamente risiedono i nostri PCB
struct list_head pcbFree_h;             // L'elemento sentinella della lista di PCB liberi


void initPcbs(void) {
    INIT_LIST_HEAD(&pcbFree_h);

    for (int i = MAXPROC-1; i >= 0; --i) {                             // Aggiunge tutti i pcb alla lista dei pcb liberi
        freePcb(&pcbFree_table[i]);
    }
}


void freePcb(pcb_t *p) {
    list_add(&p->p_next, &pcbFree_h);                               // Utilizziamo p_next per gestire la coda dei pcb liberi
}


pcb_t *allocPcb(void) {
    if (list_empty(&pcbFree_h)) {
        return NULL;
    } else {
        struct list_head* first_list_head = list_next(&pcbFree_h);
        list_del(first_list_head);                                              // Rimuoviamo il list_head dalla lista
                                                                                // recuperiamo il pcb e inizializziamo a 0/NULL tutti i valori
        struct pcb_t* first_free_pcb = container_of(first_list_head, struct pcb_t, p_next);
        first_free_pcb->priority = 0;
        first_free_pcb->p_semkey = NULL;
        first_free_pcb->p_parent = NULL;
        first_free_pcb->p_child.next = NULL;
        first_free_pcb->p_child.prev = NULL;
        INIT_LIST_HEAD(&first_free_pcb->p_next);
        INIT_LIST_HEAD(&first_free_pcb->p_sib);
        reset_state(&first_free_pcb->p_s);

        return first_free_pcb;
    }
}


void mkEmptyProcQ(struct list_head *head) {
    INIT_LIST_HEAD(head);
}


int emptyProcQ(struct list_head *head) {
    return list_empty(head);
}


void insertProcQ(struct list_head* head, pcb_t* p) {
    struct pcb_t *target_pcb;
    list_for_each_entry(target_pcb, head, p_next) {
        if (p->priority > target_pcb->priority) {                                   // Abbiamo incontrato il primo elemento con priorità minore, inseriamo p tra lui e quello precedente
            __list_add(&p->p_next, target_pcb->p_next.prev, &target_pcb->p_next);
            return;
        }
    }
    list_add_tail(&p->p_next, head);                                                // Aggiungiamo p in fondo alla lista
}


pcb_t *headProcQ(struct list_head *head) {
    if (!list_empty(head))
    {
        struct list_head* next_head = list_next(head);
        return container_of(next_head, struct pcb_t, p_next);
    }
    return NULL;
}


pcb_t *removeProcQ(struct list_head *head)
{
    if (list_empty(head)) {
        return NULL;
    } else {
        struct list_head* first_head = list_next(head);
        pcb_t *request_pcb = container_of(first_head, struct pcb_t, p_next);

        list_del(first_head);
        return request_pcb;
    }
}


pcb_t *outProcQ(struct list_head *head, pcb_t *p) {
    pcb_t *target_pcb;
    list_for_each_entry(target_pcb, head, p_next) {     // Scorriamo la lista di processi
        if (target_pcb == p)
        {
            list_del(&p->p_next);                       // se troviamo p lo rimuoviamo
            return target_pcb;
        }
    }
    return NULL;                                        // se non abbiamo trovato p ritorniamo NULL
}



// TREE FUNCTIONS -----------------------------------------------------------------------------------------------------
// La struttura ad albero è rappresentata nel modo seguente:
//      - il primo figlio di un nodo è concatenato al nodo tramite p_child
//      - il primo figlio di un nodo funge da nodo sentinella per la lista di sibling basata su p_sib
//      - la lista p_child non è circolare per facilitare controlli di parentela


int emptyChild(pcb_t *this) {
    if (this->p_child.next == NULL) return TRUE;
    else return FALSE;
}

void insertChild(pcb_t *prnt, pcb_t *p) {
    if (p != NULL && prnt != NULL) {
        p->p_parent = prnt;                                       // p ha prnt come parent, entrambi i membri dell'espressione sono puntatori pcb_t
        if (emptyChild(prnt)) {
            prnt->p_child.next = &p->p_child;
            p->p_child.prev = &prnt->p_child;
            p->p_sib.next = NULL;

            INIT_LIST_HEAD(&p->p_sib);
        } else {
            // Siccome il parent ha già un figlio aggiungiamo il nuovo pcb alla struttura come sibling nella lista in cui il primo figlio funge da sentinella
            struct list_head* first_child_head = list_next(&prnt->p_child);
            struct pcb_t* first_child_pcb = container_of(first_child_head, struct pcb_t, p_child);

            list_add_tail(&p->p_sib, &first_child_pcb->p_sib);

            p->p_child.next = NULL;
        }
    } else {
        adderrbuf("ERROR: insertChild() received invalid pointers");
        PANIC();
    }
}

pcb_t *removeChild(pcb_t *p) {
    if (emptyChild(p)) return NULL;
    else {
        struct list_head* first_child_head = p->p_child.next;
        struct pcb_t* first_child_pcb = container_of(first_child_head, struct pcb_t, p_child);

        first_child_pcb->p_parent = NULL;                                                                   // Annulliamo i puntatori inerenti alla parte di struttura che stiamo rimuovendo
        first_child_pcb->p_child.prev = NULL;

        if (!list_empty(&first_child_pcb->p_sib)) {                                                         // Se la lista di sibling non era vuota dobbiamo prendere il sibling successivo e infilarlo al posto di quello che cancelliamo nella lista di child a cui apparteneva
            struct list_head* next_sibling_head = list_next(&first_child_pcb->p_sib);
            struct pcb_t* first_sibling_pcb = container_of(next_sibling_head, struct pcb_t, p_sib);

            p->p_child.next = &first_sibling_pcb->p_child;
            first_sibling_pcb->p_child.prev = &p->p_child;

            list_del(&first_child_pcb->p_sib);                                                              // Rimuoviamo il child rimosso dalla lista di sibling a cui apparteneva

        } else {
            p->p_child.next = NULL;
        }
        return first_child_pcb;
    }
}

pcb_t *outChild(pcb_t *p) {
    struct pcb_t* parent_pcb = p->p_parent;

    if (parent_pcb==NULL) return NULL;
    else {
        if (!emptyChild(parent_pcb)) {
            struct list_head* first_child_head = parent_pcb->p_child.next;
            struct pcb_t* first_child_pcb = container_of(first_child_head, struct pcb_t, p_child);

            // Se p è il primo elemento possiamo usare removeChild(), che fa proprio questo occupandosi delle particolarità di tale situazione (il prossimo sibling deve sostituire quello cancellato nella lista gestita tramite p_child)
            if (first_child_pcb == p) {
                return removeChild(parent_pcb);
            }
            else {
                // Visto che abbiamo verificato che non era il primo figlio possiamo semplicemente rimuoverlo normalmente dalla lista di sibling
                list_del(&p->p_sib);
                return p;
            }
        }
    }
    adderrbuf("ERROR: outChild() couldn't find the received p, something must be broken with the tree structure");
    PANIC();
    return NULL;                                // Serve solo ad evitare warning, PANIC dovrebbe causare un system halt
}


struct pcb_t* nextSibling(struct pcb_t* target_sibling, struct pcb_t* first_sibling) {       // Semplice metodo per facilitare l'iterazione sulla lista di fratelli, siccome ha una struttura particolare
    if (target_sibling->p_sib.next == &first_sibling->p_sib) {
        return NULL;
    } else {
        return container_of(target_sibling->p_sib.next, struct pcb_t, p_sib);
    }
}

unsigned int get_process_index(pcb_t *p) {
    return p-pcbFree_table;
}

void print_process_queue_priorities(struct list_head* queue) {
    pcb_t *target_pcb;
    DEBUG_LOG("QUEUE:");
    list_for_each_entry(target_pcb, queue, p_next) {     // Scorriamo la lista di processi
        DEBUG_LOG_INT("P: ", target_pcb->priority);
    }
    DEBUG_LOG("\n");
};
