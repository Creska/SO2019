#ifndef ASL_H
#define ASL_H

#include "core/const.h"
#include "pcb.h"
#include "devices/terminal.h"

/* Semaphore Descriptor (SEMD) data structure */
typedef struct semd_t {
    struct list_head s_next;

    /* Semaphore key */
    int *s_key;

    /* Queue of PCBs blocked on the semaphore */
    struct list_head s_procQ;
} semd_t;

// ASL HANDLING FUNCTIONS----------------------------------------------------------------------------------------------

// Restituisce il puntatore al SEMD nella ASL la cui chiave è pari a key.
// Se non esiste tale elemento nell'ASL redtituisce NULL.
semd_t* getSemd(int *key);

// Inizializza le liste di SEMD, inserendo tutti i SEMD nella lista dei liberi
// Metodo da chiamare una sola volta in fase di inizializzazione della struttura dati
void initASL();

// Viene inserito il PCB puntato da p nella coda dei processi bloccati associata al SEMD con la chiave key.
// Se il semaforo corrispondente non è presente nella ASL, alloca un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce nella ASL, settando I campi in maniera opportuna.
// Se non è possibile allocare un nuovo SEMD perché la lista di quelli liberi è vuota, restituisce TRUE. In tutti gli altri casi, restituisce FALSE.
int insertBlocked(int *key,pcb_t* p);

// Ritorna il primo PCB dalla coda dei processi bloccati associata al SEMD della ASL con chiave key.
// Se tale descrittore non esiste nella ASL, restituisce NULL. Altrimenti, restituisce l’elemento rimosso.
// Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il descrittore corrispondente dalla ASL e lo inserisce nella coda dei descrittori liberi.
pcb_t* removeBlocked(int *key);

// Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato.
// Se il PCB non compare in tale coda, allora restituisce NULL, altrimenti, restituisce p.
// Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il descrittore corrispondente dalla ASL e lo inserisce nella coda dei descrittori liberi.
pcb_t* outBlocked(pcb_t *p);

// Restituisce (senza rimuovere) il puntatore al PCB che si trova in testa alla coda dei processi associata al SEMD con chiave key.
// Ritorna NULL se il SEMD non compare nella ASL oppure se compare ma la sua coda dei processi è vuota.
pcb_t* headBlocked(int *key);

// Rimuove il PCB puntato da p dalla coda del semaforo su cui è bloccato. Inoltre, elimina tutti i processi dell’albero radicato in p (ossia tutti i processi che hanno come avo p) dalle eventuali code dei semafori su cui sono bloccati.
void outChildBlocked(pcb_t *p);

#endif
