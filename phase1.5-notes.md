
# Inizializzazione

#### Popolamento new areas

*UMPS p13* | *UARM p23-24* (info sullo status register)

- inizializzare il PC all'indirizzo dell' **handler** del nucleo che gestisce quell'eccezione
- inizializzare `$SP` a `RAMTOP` (*RAMPTOP è un errore?*)
- inizializzare il registro di **status**: 
    - mascherare interrupts 
    - disabilitare virtual memory
    - settare kernel mode on
    
#### Inizializzazione processi
- inizializzazione strutture dati fase 1 (*solo i pcb tramite `initpcbs()`*)
- inizializzazione variabili del kernel, ora solo la lista dei processi tramite `LIST_HEAD(ready_queue)`
- inizializzazione dei PCB e dello stato dei 3 processi di test
    - interrupt abilitati
    - virtual memory OFF
    - process local timer abilitato
    - kernel mode ON
    - $SP=RAMTOP-FRAMESIZE*n
    - priorità = n
    - settare PC all'entry point dei test
- inserire i processi nella ready queue

*UMPS non fornisce una macro SYS_NEWAREA, dobbiamo definirla noi*

# Scheduler

[Context-switching](https://en.wikipedia.org/wiki/Context_switch) tra i processi con un time-slice di 3ms.

*Quindi immagino che ogni 3ms si debba controllare se c'è un processo con priorità maggiore di quello in esecuzione e nel caso switchare*

**Meccanismo di ageing** tramite il quale si evita starvation di un processo con priorità bassa. Mano a mano che il processo attende nella ready queue la sua priorità è incrementata, questa viene poi ripristinata all'originale quando il processo viene schedulato.

*Dunque dovremo aggiungere un campo `original_priority` nello struct `pcb_t` in modo da poter salvare la priorità originale del processo*

*Il punto quindi sarebbe avere un interrupt ogni 3ms dell'Interrupt Timer che innesca il controllo e l'eventuale switch nello scheduler?*

# Syscall

*UMPS p9 | UARM p15 "Software Interrupt Exceptions"*

Una syscall si distingue da un breakpoint attraverso il contenuto del registro Cause

I parametri di una syscall/break si trovano nei registri a0-a3

Nel caso delle syscall il registro a0 identifica la syscall specifica richiesta

- Creare la syscall 3 (terminate) che termina il processo corrente e tutta la sua progenie, rimuovendoli dalla ready queue.


# Gestione degli interrupt

- *UMPS p10 | UARM p14*
- *UMPS p31 note sugli interrupt per i device*
- *UMPS p34 Interrupt timer*

*Sembra vada gestito solo l'interrupt timer alla fine*
---

# Esempio strutturazione (da slide)
- scheduler.c
- handler.c
- interrupts.c
- utils.c
- main.c (*riguardo al dubbio sul main che avevamo direi che è come ipotizzavamo, fare il main a sto giro sta a noi e conterrà le chiamate di inizializzazione del sistema e di inizializzazione dei 3 processi di test*)



# TODO
- Capire il senso di FORK
- Studiarci un po' i debugger degli emulatori
- Capire il senso dell'interrupt mask (per le new area e per i pcb)
