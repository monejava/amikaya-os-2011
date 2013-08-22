#include <uMPStypes.h>
#include <types11.h>
#include <utils.h>
#include <const11.h>
#include <prgTrap.e>
#include <ssi.e>
#include <tcb.e>
#include <boot.e>
#include <scheduler.e>
#include <interrupt.e>
#include <msg.e>
#include <libumps.e>


void prgHandler() {
    
    int elapsed = GET_TODLOW - currentThread->startTime;
    currentThread->executionTime += elapsed - currentThread->elapsedTime;
    currentThread->elapsedTime = elapsed;
    copyRegState(&(currentThread->t_state), PGMTRAP_OLDAREA);

    currentThread->t_state.s_pc += WORD_SIZE;
    currentThread->t_state.status |= (STATUS_IEp | STATUS_INT_UNMASKED); /* tutti gli interrupt ablilitati */


    if(currentThread->prgMgr == NULL){
        terminate(currentThread);
    } else {
        msg_t *msg = allocMsg();
        msg->m_message = currentThread->t_state.cause;
        msg->m_sender = currentThread;
        msg->msg_ssi.service = USR_MSG;
        insertMessage(&(currentThread->sysMgr->t_inbox),msg); 

        if (outThread(&waitQueue, currentThread->sysMgr)) {
            insertThread(&readyQueue, currentThread->sysMgr);
            softBlockCount--;
        }
        stopCurrentThread();
    }
    schedule();
}
