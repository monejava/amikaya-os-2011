#ifndef _SCHEDULER_E
#define _SCHEDULER_E

#include "tcb.e"
#include "utils.h"

extern struct list_head readyQueue, waitQueue, waitForPseudoClockQueue;
extern struct list_head disk_queue[8], 
                        tape_queue[8], 
                        ethernet_queue[8], 
                        printer_queue[8],
                        terminal_queue[8];

extern tcb_t *currentThread;
extern U32 threadCount, softBlockCount;

void schedule();


/* Blocca il currentThread e al suo posto mette il  primo disponibile nella readyQueue */
static inline void stopCurrentThread() {
    insertThread(&waitQueue, currentThread);
    softBlockCount++;
    currentThread = NULL;
}

/* Toglie il thread stop dalla readyQueue e lo mette nella waitQueue */
static inline void stopThread(tcb_t *stop) {
    stop = outThread(&readyQueue,stop);
    softBlockCount++;
    insertThread(&waitQueue, stop);
}


/* Toglie il thread start dalla waitQueue e lo mette nella readyQueue */
static inline void startThread(tcb_t *start) {
    start = outThread(&waitQueue,start);
    softBlockCount--;
    insertThread(&readyQueue, start);
}

#endif
