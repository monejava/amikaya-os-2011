#include <uMPStypes.h>
#include <const11.h>
#include <types11.h>
#include <utils.h>
#include <const.h>
#include <interrupt.e>
#include <tcb.e>
#include <msg.e>
#include <ssi.e>
#include <boot.e>
#include <scheduler.e>

#include <libumps.e>

//#define DBG_INT

U32 isPseudoClock = FALSE;
cpu_t pseudoElapsed = 0;


/* se mi arriva un interrupt in reg_a1 mi arriva l'address base del device */
void fakeSend(U32 dest, U32 payload, U32 service) { 
    msg_t* msg = allocMsg();
    
    msg->m_sender = tcb_SSI;
    msg->m_message = msg->msg_ssi.payload = payload;
    msg->msg_ssi.sender = (tcb_t*)dest;
    msg->msg_ssi.reply = 0;
    msg->msg_ssi.service = service;

    insertMessage(&(tcb_SSI->t_inbox), msg);
    if (outThread(&waitQueue, tcb_SSI)) {
        insertThread(&readyQueue, tcb_SSI);
    }
}



/*funzione che fa l'ack al device che ha sollevato l'interrupt*/
void acknowledge(U32 intLineNo) {
    U32 devNo = 0,
            j = 1,
            prevStatusReg = 0,
            *bitmap = NULL,
            *deviceCommandAck = NULL,
            *statusRegister = NULL;

    /* dobbiamo :
    1. trovare qual e' il device number del device che ha sollevato un interrupt
                    a. guardare nella interrupting devices bitmap, vedendo se ogni bit 
                            partendo dal piu' basso e' = 1, appena ne trovo uno mi fermo
    2. scrivere 1 nel suo command register */

    /* calcolo l'indirizzo della pending interrupts bitmap del device */
    bitmap = (U32 *) (PENDING_BITMAP_START + (WORD_SIZE * (intLineNo - 3)));


    /* 128 = 2^7, perche' ci sono 8 devType per ciascuna linea
     * (il primo e' 2^0)
     * Facendo lo shift verso destra di un U32 con valore 1.
     * e mettendolo in & binario con la bitmap, verifichiamo se
     * il device in questione ha il bit settato o meno */
    for (; j < 128; j <<= 1, devNo++) {
        if ((*bitmap & j) == 1) /* ho trovato un interrupting device, in devNo c'e' il suo index */
            break;
    }

    /* Device Address Base = 0x1000.0050 + ((IntlineNo - 3) * 0x80) + (DevNo * 0x10) 
     * e trovo la base del device register */
    if (intLineNo != INT_TERMINAL) {

        /* devo trovare lo status, che e' (base) + 0x0 */
        statusRegister = (U32 *) DEV_ADDR_BASE(intLineNo, devNo);

        prevStatusReg = *statusRegister & STATUSMASK;

        /* devo trovare il command, che e' status + 0x4 */
        deviceCommandAck = (U32 *) &(statusRegister[1]);
        *deviceCommandAck = DEV_C_ACK;

    } else { /* il terminal ha due registri command */

        /* dobbiamo:
         * sapere se era una transm o una recv o entrambe
         * leggere il registro transm/recv status, vedere se c'e' 5
         * scrivere ack nel registro transm/recv command
         */
        /* trovo il registro status del terminale in questione */
        statusRegister = (U32 *) (TERMINAL0ADDR + (devNo * DEVREGSIZE) + (RECVSTATUS * DEVREGLEN));

        /* devo leggere il primo byte dello status */
        if ((*statusRegister & STATUSMASK) == DEV_TRCV_S_CHARRECV) {

            prevStatusReg = *statusRegister & STATUSMASK;

            deviceCommandAck = (U32 *) &(statusRegister[1]);
            *deviceCommandAck = DEV_C_ACK;
        }

        /* era (solo/anche) una transm */
        statusRegister = (U32 *) (TERMINAL0ADDR + (devNo * DEVREGSIZE) + (TRANSTATUS * DEVREGLEN));

        if ((*statusRegister & STATUSMASK) == DEV_TTRS_S_CHARTRSM) {
            prevStatusReg = *statusRegister & STATUSMASK;
            deviceCommandAck = (U32 *) &(statusRegister[1]);
            *deviceCommandAck = DEV_C_ACK;
        }
    }
    fakeSend((U32) DEV_ADDR_BASE(intLineNo, devNo), (U32) prevStatusReg, WAKE_UP_FROM_IO);
}

void ftimer() {
    cpu_t elapsed = 0;
    if ((elapsed = GET_TODLOW - pseudoElapsed) >= SCHED_PSEUDO_CLOCK ) {        
        isPseudoClock = FALSE;
        pseudoElapsed = 0;
        if (!emptyThreadQ(&waitForPseudoClockQueue))
            fakeSend(0, 0, WAKE_UP_PSEUDO_CLOCK); /*messaggio all'SSI per risvegliare i thread in attesa di uno pseduo clock*/
    } else {
        pseudoElapsed += elapsed;
    }    
}

void intsHandler() {

    U32 timer, disk, tape, printer, terminal; // aggiungere l'ethernet?????
    
    if (currentThread != NULL) {
        cpu_t elapsed = GET_TODLOW - currentThread->startTime;
        currentThread->executionTime += elapsed - currentThread->elapsedTime;
        currentThread->elapsedTime = elapsed;
        copyRegState(&(currentThread->t_state), INT_OLDAREA);
    }
    

    /* ritorna 1 se era settata la interrupt line X */
    timer = CAUSE_IP_GET(((state_t *) INT_OLDAREA)->cause, INT_TIMER);
    disk = CAUSE_IP_GET(((state_t *) INT_OLDAREA)->cause, INT_DISK);
    tape = CAUSE_IP_GET(((state_t *) INT_OLDAREA)->cause, INT_TAPE);
    //aggiungere ethernet
    printer = CAUSE_IP_GET(((state_t *) INT_OLDAREA)->cause, INT_PRINTER);
    terminal = CAUSE_IP_GET(((state_t *) INT_OLDAREA)->cause, INT_TERMINAL);

    if (timer == INTERRUPT_TYPE(INT_TIMER)) {
        ftimer();
    } else if (disk == INTERRUPT_TYPE(INT_DISK)) {
        acknowledge(INT_DISK);
    } else if (tape == INTERRUPT_TYPE(INT_TAPE)) {
        acknowledge(INT_TAPE);
    } else if (printer == INTERRUPT_TYPE(INT_PRINTER)) {
        acknowledge(INT_PRINTER);
    } else if (terminal == INTERRUPT_TYPE(INT_TERMINAL)) {
        acknowledge(INT_TERMINAL);
    }

    schedule();
}
