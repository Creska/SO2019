#include "core/processes/asl.h"


semd_t semd_table[MAXPROC];
struct list_head semdFree_h;        // Elemento sentinella della lista di semd liberi
struct list_head semd_h;            // Elemento sentinella della lista ASL (Active Semaphore List)

/* ASL handling functions */
semd_t* getSemd(int *key) {
    semd_t* semd;
    list_for_each_entry(semd, &semd_h, s_next) {
        if (semd->s_key==key) return semd;
    }
    return NULL;                                            // In questo modo è O(n), può essere ottimizzata ad O(log n) nelle fasi successive se necessario
}

void initASL() {
    INIT_LIST_HEAD(&semd_h);

    INIT_LIST_HEAD(&semdFree_h);
    for (int i = 0; i < MAXPROC; ++i) {                     // Inizializza i concatenatori dei semd e li aggiunge alla lista dei liberi
        INIT_LIST_HEAD(&semd_table[i].s_next);
        INIT_LIST_HEAD(&semd_table[i].s_procQ);
        list_add(&semd_table[i].s_next, &semdFree_h);
    }
}

semd_t* retrieve_free_semd(int* key, pcb_t* p) {
    if (list_empty(&semdFree_h)) {
        return NULL;
    } else {                                                                           // La lista dei semd liberi non è vuota, ne spostiamo uno nella ASL e settiamo i suoi campi
        struct list_head* first_free_semd_head = list_next(&semdFree_h);
        list_del(first_free_semd_head);
        list_add(first_free_semd_head, &semd_h);                                       // Spostiamo un semd dalla lista free all'ASL

        semd_t* target_semd = container_of(first_free_semd_head, struct semd_t, s_next);
        target_semd->s_key = key;
        p->p_semkey = key;
        INIT_LIST_HEAD(&target_semd->s_procQ);
        return target_semd;
    }
}

int insertBlockedFifo(int* key, pcb_t* p) {
    struct semd_t* target_semd = getSemd(key);
    if (target_semd == NULL) {                                                              // Il semaforo corrispondente non è presente nella ASL
        target_semd = retrieve_free_semd(key, p);
        if (target_semd==NULL) return TRUE;
    }

    list_add_tail(&p->p_next, &target_semd->s_procQ);
    return FALSE;
}

int insertBlocked(int *key,pcb_t* p) {
    struct semd_t* target_semd = getSemd(key);
    if (target_semd == NULL) {                                                              // Il semaforo corrispondente non è presente nella ASL
         target_semd = retrieve_free_semd(key, p);
         if (target_semd==NULL) return TRUE;
    }

    insertProcQ(&target_semd->s_procQ, p);                                  // Ora che abbiamo il semd appropriato aggiungiamo il pcb alla sua lista dei processi
    return FALSE;
}

// Sposta il target_semd nella lista dei semd liberi se la sua coda dei processi è vuota
void freeSemdIfEmpty(struct semd_t* target_semd) {
    if (list_empty(&target_semd->s_procQ)) {                                // Siccome in seguito alla rimozione la lista di processi è vuota ritorniamo il semd alla lista dei liberi
        list_del(&target_semd->s_next);
        list_add(&target_semd->s_next, &semdFree_h);
    }
}

pcb_t* removeBlocked(int *key) {
    struct semd_t* target_semd = getSemd(key);                              // Se non troviamo un semd corrispondente a key ritorniamo NULL
    if (target_semd==NULL) {
        return NULL;
    } else {
        struct pcb_t* target_pcb = removeProcQ(&target_semd->s_procQ);
        if (target_pcb==NULL) {                                             // removeProcQ ha ritornato NULL, quindi la lista di processi era vuota
            return NULL;
        } else {
            target_pcb->p_semkey = NULL;
            freeSemdIfEmpty(target_semd);                                   // Liberiamo il semd se la sua coda di processi ora è vuota
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
        freeSemdIfEmpty(target_semd);                           // Liberiamo il semd se la sua coda di processi ora è vuota
        return removed_pcb;
    }
}

pcb_t* headBlocked(int *key) {
    struct semd_t* target_semd = getSemd(key);
    if (target_semd==NULL || list_empty(&target_semd->s_procQ)) {                       // Controlliamo che il semd corrispondente a key esista e che ci sia un processo bloccato su di lui
        return NULL;
    } else {
        return container_of(list_next(&target_semd->s_procQ), struct pcb_t, p_next);
    }
}

void outChildBlocked(pcb_t *p) {
    outBlocked(p);                                                  // visitiamo in prevista i nodi dell'albero e applichiamo outBlocked(p)
    if (!emptyChild(p)) {
        struct pcb_t* first_pcb = container_of(list_next(&p->p_child), struct pcb_t, p_child);

        struct pcb_t* target_pcb = first_pcb;
        do {
            outChildBlocked(target_pcb);                            // chiamiamo ricorsivamente outChildBlocked() per ogni figlio di p
            target_pcb = nextSibling(target_pcb, first_pcb);
        } while (target_pcb!=NULL);                                 // utilizziamo questa logica piuttosto che le macro di listx, perchè la lista di child è gestita in maniera particolare
    }
}
