#include "asl.h"


semd_t semd_table[MAXPROC];
struct list_head semdFree_h;        // Elemento sentinella della lista di semd liberi
struct list_head semd_h;            // Elemento sentinella della lista ASL (Active Semaphore List)

/* ASL handling functions */
semd_t* getSemd(int *key) {
    struct list_head* p;
    list_for_each(p, &semd_h) {
        semd_t* semd = container_of(p, struct semd_t, s_next);
        if (semd->s_key==key) return semd;
    }
    return NULL;                    // In questo modo è O(n), probabilmente può essere ottimizzata a O(log n) con un po' di sbatto, ci si pensa se abbiamo tempo
}

void initASL() {
    INIT_LIST_HEAD(&semd_h);

    INIT_LIST_HEAD(&semdFree_h);
    for (int i = 0; i < MAXPROC; ++i) {
        INIT_LIST_HEAD(&semd_table[i].s_next);
        list_add(&semd_table[i].s_next, &semdFree_h);

        INIT_LIST_HEAD(&semd_table[i].s_procQ);
    }
}

int insertBlocked(int *key,pcb_t* p) {
    struct semd_t* target_semd = getSemd(key);
    if (target_semd == NULL) {                       // Il semaforo corrispondente non è presente nella ASL
         if (list_empty(&semdFree_h)) {
             return TRUE;
         } else {
             // La lista dei semd liberi non è vuota, ne spostiamo uno nella ASL e settiamo i suoi campi
             struct list_head* first_free_semd_head = list_next(&semdFree_h);
             list_del(first_free_semd_head);
             list_add(first_free_semd_head, &semd_h);                                                       // Spostiamo un semd dalla lista free all'ASL

             target_semd = container_of(first_free_semd_head, struct semd_t, s_next);
             target_semd->s_key = key;
             p->p_semkey = key;                                                                             // FIXME Dobbiamo occuparci noi di aggiornare semkey nel processo? penso di si
             INIT_LIST_HEAD(&target_semd->s_procQ);
         }
    }

    // Ora che abbiamo il semd appropriato aggiungiamo il pcb alla sua lista dei processi
    insertProcQ(&target_semd->s_procQ, p);
    return FALSE;
}

void freeSemdIfEmpty(struct semd_t* target_semd) {
    if (list_empty(&target_semd->s_procQ)) {                        // siccome in seguito alla rimozione la lista di processi è vuota ritorniamo il semd alla lista dei liberi
        list_del(&target_semd->s_next);
        list_add(&target_semd->s_next, &semdFree_h);
    }
}

pcb_t* removeBlocked(int *key) {
    struct semd_t* target_semd = getSemd(key);
    if (target_semd==NULL) {
        return NULL;
    } else {
        struct pcb_t* target_pcb = removeProcQ(&target_semd->s_procQ);      // rimuoviamo il
        if (target_pcb==NULL) {                                             // removeProcQ ha ritornato NULL, quindi la lista di processi era vuota
            return NULL;                                                    // FIXME forse questo check non serve perchè la coda non può essere vuota a questo punto?
        } else {
            target_pcb->p_semkey = NULL;
            freeSemdIfEmpty(target_semd);
            return target_pcb;
        }
    }
}

pcb_t* outBlocked(pcb_t *p) {
    struct semd_t* target_semd = getSemd(p->p_semkey);
    if (target_semd==NULL) {
        return NULL;
    } else {
        struct pcb_t* removed_pcb = outProcQ(&target_semd->s_procQ, p);
        if (removed_pcb!=NULL) {
            removed_pcb->p_semkey = NULL;
        }
        freeSemdIfEmpty(target_semd);
        return removed_pcb;
    }
}

pcb_t* headBlocked(int *key) {
    struct semd_t* target_semd = getSemd(key);
    if (target_semd==NULL || list_empty(&target_semd->s_procQ)) {
        return NULL;
    } else {
        struct list_head* first_head = list_next(&target_semd->s_procQ);
        return container_of(first_head, struct pcb_t, p_next);
    }
}

void outChildBlocked(pcb_t *p) {
    outBlocked(p);
    if (!emptyChild(p)) {
        struct list_head* first_child_head = list_next(&p->p_child);
        struct pcb_t* first_pcb = container_of(first_child_head, struct pcb_t, p_child);

        struct pcb_t* target_pcb = first_pcb;
        do {
            outChildBlocked(target_pcb);
            target_pcb = nextSibling(target_pcb, first_pcb);
        } while (target_pcb!=NULL);
    }
}
