#include <uMPStypes.h>
#include <types11.h>
#include <utils.h>
#include <const11.h>

#include <ssi.e>
#include <tcb.e>
#include <boot.e>
#include <scheduler.e>
#include <interrupt.e>
#include <libumps.e>
#include <p2test.0.2.e>

tcb_t *sender;
device_t deviceData;

/* questa funz mi serve per scoprire in base all'address dello status
 * (che è quello che viene passato alla SSIRequest in caso di IO)
 * in che lista mettere il current thread */
struct list_head* select_io_queue_from_status_addr(memaddr status_addr) {

    int dev_number = DEVICE_N_FROM_REGSTATUS(status_addr);
    if (IS_DISK_DEVICE(status_addr)) {
        return &(disk_queue[dev_number]);
    } else if (IS_TAPE_DEVICE(status_addr)) {
        return &(tape_queue[dev_number]);
    } else if (IS_ETHERNET_DEVICE(status_addr)) {
        return &(ethernet_queue[dev_number]);
    } else if (IS_PRINTER_DEVICE(status_addr)) {
        return &(printer_queue[dev_number]);
    } else { // if(IS_TERMINAL_DEVICE(status_addr))
        return &(terminal_queue[dev_number]);
    }
}

/* Crea un fratello al thread sender, il fratello deve avere come stato inziale 
 * lo stato passato come parametro. Se la creazione va a buon fine ritorna 
 * l'address del figlio altrimenti restituisce CREATENOGOOD*/
tcb_t * createBrother(state_t *state) {

    tcb_t *brother = allocTcb();

    if (brother == NULL)
        return (tcb_t*) CREATENOGOOD;

    if (sender->t_parent == NULL)
        sender->t_parent = tcb_init;

    /* imposto il nuovo stato al thread appena creato */
    copyRegState(&(brother->t_state), state);
    /* lo inserisco nella lista dei thread pronti e incremento il numero di essi */
    insertThread(&readyQueue, brother);
    /* Lo pongo come fratello del sender */
    insertChild(sender->t_parent, brother);
    threadCount++;
    return brother;
}

/* Crea un figlio al thread sender, il fratello deve avere come stato inziale 
 * lo stato passato come parametro. Se la creazione va  a buon fine ritorna 
 * l'address del figlio altrimenti restituisce CREATENOGOOD*/
tcb_t * createSon(state_t *state) {

    tcb_t * son = allocTcb();

    if (son == NULL)
        return (tcb_t*) CREATENOGOOD;

    /*imposto il nuovo stato al thread appena creato*/
    copyRegState(&(son->t_state), state);
    /* lo inserisco nella lista dei thread pronti e incremento il numero di essi*/
    insertThread(&readyQueue, son);
    /* Lo pongo come figlio del sender*/
    insertChild(sender, son);
    threadCount++;
    return son;
}

/* Rimuovo il thread dalla lista dei thread pronti, dei thread in attesa di IO
   e da quelli in attesa di pseudoclock */
void terminate(tcb_t* killed) {
    tcb_t *child;
    
    //elimino ricorsivamente la progenie
    while((child = removeChild(killed)) != NULL)
        terminate(child);
    
    outChild(killed);
    
    if(killed == currentThread) {
        freeTcb(killed);
        currentThread = NULL;
        threadCount--;
    } else {
        if(outThread(&waitQueue,killed) != NULL || outThread(&waitForPseudoClockQueue,killed) != NULL)
            softBlockCount--;
        else if(outThread(&readyQueue, killed) == NULL)//se non si trova in nessuna delle 3 code PANIC
            PANIC();
            
        threadCount--;
        freeTcb(killed);
        killed = NULL;
    }
}

U32 specPrgMgr(tcb_t* mgr) {
    if (sender->prgMgr != NULL || mgr == NULL) {
        terminate(sender);
        return FALSE;
    } else {
        sender->prgMgr = (tcb_t *) mgr;
        return TRUE;
    }
}

U32 specTlbMgr(tcb_t* mgr) {
    if (sender->tlbMgr != NULL || mgr == NULL) {
        terminate(sender);
        return FALSE;
    } else {
        sender->tlbMgr = mgr;
        return TRUE;
    }
}

U32 specSysMgr(tcb_t* mgr) {
    if (sender->sysMgr != NULL || mgr == NULL){
        terminate(sender);
        return FALSE;
    } else {
        sender->sysMgr = mgr;
        return TRUE;
    }
}

cpu_t getCpuTime(tcb_t *tcb) {
    return tcb->executionTime;
}

U32 waitForClock(tcb_t *tcb) {
    list_del(&(tcb->t_next));
    mkEmptyThreadQ(&(tcb->t_next));
    insertThread(&waitForPseudoClockQueue, tcb);
    if (isPseudoClock == FALSE) {
        isPseudoClock = TRUE;
        pseudoElapsed = GET_TODLOW;
    }
    return FALSE;
}

U32 wakeUpPseudoClock(void) {
    tcb_t*temp;
    while ((temp = removeThread(&waitForPseudoClockQueue)) != NULL) {
        insertThread(&readyQueue, temp);
        MsgSend(SEND, temp, 0);
        softBlockCount--;
    }
    return FALSE;
}

U32 waitForIO(ssimsg_t * msg_ssi) {
    int dev_number = DEVICE_N_FROM_REGSTATUS(msg_ssi->payload);
    if (deviceData.term[dev_number].exist == TRUE) {
        msg_ssi->reply = deviceData.term[dev_number].payload;
        deviceData.term[dev_number].exist = FALSE;
        MsgSend(SEND, msg_ssi->sender, msg_ssi);
    } else {
        outThread(&readyQueue, msg_ssi->sender);
        struct list_head *waitQueueForDev = select_io_queue_from_status_addr(msg_ssi->payload);
        insertThread(waitQueueForDev, msg_ssi->sender);
    }
    return FALSE;
}

U32 wakeUpFromIO(ssimsg_t * msg_ssi) {
    struct list_head *waitQueueForDev = select_io_queue_from_status_addr((memaddr) msg_ssi->sender);
    if (!list_empty(waitQueueForDev)) {
        tcb_t *first = removeThread(waitQueueForDev);
        insertThread(&waitQueue, first);
        msg_ssi->reply = msg_ssi->payload;
        MsgSend(SEND, first, msg_ssi);
        return FALSE;
    } else {
        int dev_number = DEVICE_N_FROM_REGSTATUS(msg_ssi->payload);
        deviceData.term[dev_number].payload = msg_ssi->payload;
        deviceData.term[dev_number].exist = TRUE;
        return FALSE;
    }
}

/* Tramite questa funzione un thread può richiedere un servizio. Se la richiesta 
 * fatta dal Thread non esiste allora esso e tutta la sua progenie verranno uccisi.
 * Il thread che fa la richiesta deve obbligatoriamente restare in attesa di una 
 * risposta.
 * 
 * service: codice mnemonico che identifica il servizio
 * payload: contiene un'argomento, se richiesto, per il servizio
 * reply: punterà all'area dove verra messa la risposta nel caso sia richiesta
 */
void SSIRequest(U32 service, U32 payload, U32 *reply) {
    ssimsg_t msg_ssi;
    
    msg_ssi.service = service;
    msg_ssi.payload = payload;
    msg_ssi.sender = currentThread;

    MsgSend(SEND, SSI_MAGIC, (U32) & msg_ssi);
    MsgRecv(RECV, SSI_MAGIC, (U32) & msg_ssi);

    *reply = msg_ssi.reply;
}

U32 SSIdoRequest(ssimsg_t * msg_ssi, U32 *reply) {

    U32 service = msg_ssi->service;
    U32 payload = msg_ssi->payload;
    sender = msg_ssi->sender;

    if (service < 1 || service > MAX_REQUEST_VALUE)/* Uccidire il thread chiamante*/
        terminate(sender);

    switch (service) {
            /* service request values */
        case CREATEBROTHER:
            *reply = (U32) createBrother((state_t *) payload);
            break;
        case CREATESON:
            *reply = (U32) createSon((state_t *) payload);
            break;
        case TERMINATE:
            terminate(sender);
            break;
        case GETCPUTIME:
            *reply = (U32) getCpuTime(sender);
            break;
        case SPECPRGMGR:    return specPrgMgr((tcb_t *)payload);
        case SPECTLBMGR:    return specTlbMgr((tcb_t *)payload);
        case SPECSYSMGR:    return specSysMgr((tcb_t *)payload);
        case WAITFORCLOCK:  return waitForClock(sender);
        case WAITFORIO:     return waitForIO(msg_ssi);
        case WAKE_UP_PSEUDO_CLOCK:   return wakeUpPseudoClock();
        case WAKE_UP_FROM_IO:   return wakeUpFromIO(msg_ssi);

    }
    return TRUE;
}

void SSI_function_entry_point() {
    U32 toBeSent;
    ssimsg_t msg;
    for (;;) {

        sender = MsgRecv(RECV, ANYMESSAGE, &msg);
        toBeSent = SSIdoRequest(&msg, &(msg.reply));

        if (toBeSent)
            MsgSend(SEND, (U32) sender, &msg);
    }
}
