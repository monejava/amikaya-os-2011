/*************/
/* Scheduler */
/*************/

#include <const.h>
#include <types11.h>
#include <const11.h>


#include <scheduler.e>
#include <interrupt.e>
#include <tcb.e>
#include <msg.e>
#include <ssi.e>
#include <boot.e>
#include <libumps.e>
#include <p2test.0.2.e>

tcb_t *currentThread = NULL;

LIST_HEAD(readyQueue); /* la coda dei thread pronti per essere eseguiti */
LIST_HEAD(waitQueue); /* la coda dei thread in attesa di essere sbloccati */
LIST_HEAD(waitForPseudoClockQueue); /* la coda dei thread in attesa di pseudoclock*/

struct list_head disk_queue[8] = {LIST_HEAD_INIT_ARRAY_8(disk_queue)};
struct list_head tape_queue[8] = {LIST_HEAD_INIT_ARRAY_8(tape_queue)};
struct list_head ethernet_queue[8] = {LIST_HEAD_INIT_ARRAY_8(ethernet_queue)};
struct list_head printer_queue[8] = {LIST_HEAD_INIT_ARRAY_8(printer_queue)};
struct list_head terminal_queue[8] = {LIST_HEAD_INIT_ARRAY_8(terminal_queue)};

U32 threadCount = 2, /* il numero di thread nel sistema */
        softBlockCount = 0; /* il numero di thread bloccai in attesa di I/O o di completare una richiesta dal SSI */

void prova(){}

/**************************************************************/
/* Funzione che deve gestire clock, pseudo-clock e scheduling */

/**************************************************************/
void schedule() {

    /* azioni da compiere quando non c'e' nessun thread pronto ad eseguire */
    if (emptyThreadQ(&readyQueue) && currentThread == NULL) {
        prova();
        
        if (threadCount == 1)/* se c'e' solo il SSI -> normal system shutdown */
            HALT(); /* chiamo la HALT ROM routine */
        else if (threadCount > 0 && softBlockCount == 0) {/* deadlock */
            PANIC(); /* chiamo la PANIC ROM routine */
        } else if (threadCount > 0 && softBlockCount > 0) { /* in attesa di un interrupt -> wait state */
            /* se ci sono thread in attesa dello pseudo tick,
             * carico il valore dello pseudo clock nel registro della cpu.*/
            if (!emptyThreadQ(&waitForPseudoClockQueue)) {
                SET_IT(SCHED_PSEUDO_CLOCK);
            }
            /* impostiamo lo stato del processore con gli interrupt abilitati*/
            setSTATUS(getSTATUS() | STATUS_IEc | STATUS_INT_UNMASKED);
            for (;;);
        }
    } else {
        /* Se non c'è nessun Thread in esecuzione ma c'e n'è almeno uno nella readyQueue allora 
           carico un thread*/
        if (currentThread == NULL) {
            currentThread = removeThread(&readyQueue);
            currentThread->elapsedTime = 0;
            currentThread->startTime = GET_TODLOW;
            SET_IT(SCHED_TIME_SLICE);
            /* Altrimenti se è passato il SCHED_TIME_SLICE rimuovo il thread corrente dall'esecuzione*/
        } else if (currentThread->elapsedTime >= SCHED_TIME_SLICE) {
            //in questo modo do priorità all'SSI
            if (currentThread != tcb_SSI) {
                insertThread(&readyQueue, currentThread);
                /*Carico un nuovo thread*/
                currentThread = removeThread(&readyQueue);
            }

            currentThread->elapsedTime = 0;
            currentThread->startTime = GET_TODLOW;

            /* Se e' scattato lo pseudo clock non settiamo il timer a 5 ms 
             * dato che scattera' subito l'interrupt dello pseudo clock */
            if (!isPseudoClock)
                SET_IT(SCHED_TIME_SLICE);

        }
        /* carico lo stato del thread nel processore  dalla sua tcb */
        LDST(&(currentThread->t_state));
    }
}
