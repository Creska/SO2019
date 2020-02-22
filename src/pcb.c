#include "pcb.h"
#include "listx.h"
#include "const.h"
#include "types_bikaya.h"

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

/*Insert the given pcb in the head of the list*/
void freePcb(pcb_t * p)
{   
    list_add(&p->p_next, &pcbFree_h);
}

/*Initialize the mkEmptyProcQ's queue putting the buddy's element*/
void mkEmptyProcQ(struct list_head *head)
{
    INIT_LIST_HEAD(head);
    list_add_tail(head, &head->next);
}

/*If the buddy's next pointer is equal to NULL the emptyProcQ is empty*/
int emptyProcQ(struct list_head *head)
{
    return &head->next == NULL;
}

/*Iterate each element of the queue. If the element to insert has high priority compared to the current pcb we add it between
  the previous element and the current pcb instead if che element to insert has less priority it will be put between the current 
  pcb and his next element*/
void insertProcQ(struct list_head* head, pcb_t* p)
{
    struct list_head *next_element;
    
    list_for_each(next_element, head)
    {
        pcb_t *check = container_of(next_element, PCBlistItem, p_next);
        int current_priority =  &check->priority;

        if(&p->priority > &check->priority)
        {
            __list_add(&p->p_next, &check->p_next.prev, &check->p_next);
        }
        else
        {
            __list_add(&p->p_next, &check->p_next, &check->p_next.next);
        }
    }
}

/*If the queue is not empty we return the pcb's pointer of the first element that is not the buddy otherwise NULL*/
pcb_t *headProcQ(struct list_head *head)
{
    if (!list_empty(head))
    {
        return container_of(&head->next, PCBlistItem, p_next);
    }

    return NULL;
}

/*Set the pointer request_pcb to the next item of the buddy. If the pcb is not the buddy we can remove it otherwise the queue is empty
  and we will return the NULL pointer's value*/
pcb_t *removeProcQ(struct list_head *head)
{
    pcb_t *request_pcb = list_next(head);

    if (request_pcb != head)
    {
        list_del(request_pcb);

        return request_pcb;
    }

    return NULL;
}

/*Iterate each element of the queue. If the element is inside the structure the pointer called pcb_requested is set to his memory address
  and deleted from the queue instead if the pcb is not present inside the queue we can return the pcb_requested pointer set by default to NULL.*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p)
{
    pcb_t *pcb_removed;
    struct list_head *next_element;

    list_for_each(next_element, head)
    {
        
        pcb_removed = container_of(next_element, PCBlistItem, p_next);
            
        if (pcb_removed == &p)
        {
            list_del(p);
        }
    }

    return pcb_removed;
}



/* Tree view functions */

// Implementazione di prova con struttura ad albero come da sketch che vi ho mandato su Telegram.
// La lista di child è trattata in maniera leggermente diversa dal solito, perchè l'ultimo elemento punta a NULL invece che alla sentinella

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
    if (emptyChild(p)) return NULL;
    else {
        struct list_head* first_child_head = p->p_child.next;
        struct pcb_t* first_child_pcb = container_of(first_child_head, struct pcb_t, p_child);

        // Annulliamo i puntatori (inerenti all'albero) del pcb che stiamo rimuovendo
        first_child_pcb->p_parent = NULL;
        first_child_pcb->p_child.next = NULL;
        first_child_pcb->p_child.prev = NULL;

        // Se la lista di sibling non era vuota dobbiamo prendere il sibling successivo e infilarlo al posto di quello che cancelliamo nella lista di child a cui apparteneva
        if (!list_empty(&first_child_pcb->p_sib)) {
            struct list_head* next_sibling_head = list_next(&first_child_pcb->p_sib);
            struct pcb_t* first_sibling_pcb = container_of(next_sibling_head, struct pcb_t, p_sib);

            p->p_child.next = &first_sibling_pcb->p_child;
            first_sibling_pcb->p_child.prev = &p->p_child;

        } else {
            // Se il pcb cancellato non aveva sibling dal punto di vista di p il "next child" ora è NULL
            p->p_child.next = NULL;
        }

        return first_child_pcb; // Sulle slide non è chiarissimo ma immagino vada retornato un puntatore al pcb eliminato
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
            if (first_child_pcb == p) removeChild(parent_pcb);
            else {
                // Visto che abbiamo verificato che non era il primo figlio possiamo semplicemente rimuoverlo normalmente dalla lista di sibling
                // TODO Forse ci vuole un controllo per verificare che p ci sia nella lista? In teoria dovrebbe essere sempre vero se il resto del sistema funziona
                list_del(&p->p_sib);
            }
        }
    }

    // TODO error message, couldn't find p
}
