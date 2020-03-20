
# Inizializzazione

#### Popolamento new areas

*UMPS p13* | *UARM p23-24* (info sullo status register)

- inizializzare il PC all'indirizzo dell' **handler** del nucleo che gestisce quell'eccezione
- inizializzare `$SP` a `RAMPTOP`
- inizializzare il registro di **status**: 
    - mascherare interrupts             *(mascherarli in che modo? Forse la risposta è nella slide 25 delle specifiche*)
    - disabilitare virtual memory
    - settare kernel mode on
    - abilitare un timer
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



## Esempio strutturazione (da slide)
- scheduler.c
- handler.c
- interrupts.c
- utils.c
- main.c (*riguardo al dubbio sul main che avevamo direi che è come ipotizzavamo, fare il main a sto giro sta a noi e conterrà le chiamate di inizializzazione del sistema e di inizializzazione dei 3 processi di test*)
