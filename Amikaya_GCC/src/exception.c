
#include <types11.h>
#include <uMPS_include/uMPStypes.h>
#include <utils.h>

#include <msg.e>
#include <ssi.e>
#include <boot.e>
#include <scheduler.e>
#include <interrupt.e>
#include <exception.e>
#include <p2test.0.2.e>
#include <libumps.e>


U32 cause;

void sysBpHandler() {

    copyRegState(&(currentThread->t_state), SYSBK_OLDAREA);

    currentThread->t_state.status |= (STATUS_IEp | STATUS_INT_UNMASKED); /* tutti gli interrupt ablilitati */

    /* lettura di cause.excode per distinguere tra sys e bp */
    cause = CAUSE_EXCCODE_GET(currentThread->t_state.cause);

    /* se KuMode = 1 il thread e' in user mode */
    if (cause == EXC_SYSCALL) {
        if(IS_KU_MODE(((state_t*) SYSBK_OLDAREA)->status)) { 
        
            U32 syscall_code = ((state_t*) SYSBK_OLDAREA)->reg_a0;

            /*********************************** SEND ***********************************************/
            if (syscall_code == SEND) {
                /* testo del messaggio*/
                U32 payload = currentThread->t_state.reg_a2;

                /*Creo il messaggio*/
                msg_t *msg = allocMsg();

                if (msg != NULL) {
                    /*Indirizzo del tcb di destinazione*/
                    tcb_t *dest = (tcb_t *) currentThread->t_state.reg_a1;

                    /* se la send arriva dal SSIRequest con sender SSI_MAGIC capisco che è indirizzato alla
                     * SSI e inserisco il messaggio alla ssi! */
                    if ((U32) (dest) == SSI_MAGIC) {

                        ssimsg_t *temp_ssimsg = (ssimsg_t*) payload;

                        msg->m_sender = temp_ssimsg->sender; //currentThread
                        msg->m_message = temp_ssimsg->payload;

                        memcopy(&(msg->msg_ssi), (void*) temp_ssimsg, sizeof (ssimsg_t));
                        insertMessage(&(tcb_SSI->t_inbox), msg);


                        if (outThread(&waitQueue, tcb_SSI)) {
                            insertThread(&readyQueue, tcb_SSI);
                            softBlockCount--;
                        }
                    //Controllo se è una send di un trapmanager che decide se uccidere o continuare
                    //il processo che ha causato la trap
                    } else if(dest->prgMgr == currentThread ||
                              dest->tlbMgr == currentThread ||
                              dest->sysMgr == currentThread) {
                        
                        
                        if(payload == TRAPCONTINUE) {
                            startThread(dest); 
                        } else if (payload == TRAPTERMINATE) {
                            terminate(dest);
                            if(currentThread == NULL)
                                schedule();
                        }

                    } else { /* altrimenti è una send di un thread normale */

                        msg->m_message = payload;

                        if (currentThread == tcb_SSI) {
                            msg->m_sender = (tcb_t *) SSI_MAGIC;
                            memcopy(&(msg->msg_ssi), (void*) payload, sizeof (ssimsg_t));
                        } else {
                            msg->m_sender = currentThread;
                            msg->msg_ssi.service = USR_MSG;
                        }

                        insertMessage(&(dest->t_inbox), msg);
                        if (outThread(&waitQueue, dest)) {
                            insertThread(&readyQueue, dest);
                            softBlockCount--;
                        }
                    }
                    currentThread->t_state.reg_v0 = MSGGOOD;
                } else {
                    currentThread->t_state.reg_v0 = MSGNOGOOD;
                }

                /* Evito che rientri nel codice della syscall*/
                currentThread->t_state.s_pc += WORD_SIZE;
                LDST(&currentThread->t_state);

            /*********************************** RECV *******************************************/
            } else if (syscall_code == RECV) {
                /* Indirizzo del tcb dal quale ci aspettimo un messaggio oppure ANYMESSAGE*/
                tcb_t* sender = (tcb_t *) currentThread->t_state.reg_a1;
                /* Dove verrà memorizzato il messaggio*/
                S32 *reply = (S32 *) currentThread->t_state.reg_a2;

                /* Prende dalla inbox del current thread il primo messaggio dal sender
                 sepecificato, se sender == ANYMESSAGE allora restituisce il primo messaggio
                 disponibile nella coda (ANYMESSAGE=0=NULL)*/
                msg_t *msg = popMessage(&(currentThread->t_inbox), sender);

                if (msg == NULL) {
                    stopCurrentThread();
                    /* Da notare che non incremento il pc in modo tale che quando il thread 
                       viene sbloccato richiami nuovamente la syscall (nella speranza che ci
                       il messaggio).*/
                } else {

                    //se il messaggio è indirizzato al
                    if (msg->msg_ssi.service != USR_MSG) {
                        memcopy(reply, &(msg->msg_ssi), sizeof (ssimsg_t));
                    } else { /* Metto il messaggio nella zona designata */
                        *reply = msg->m_message;
                    }

                    /* metto il tcb del sender nel registro usato come 
                     * valore di ritorno della funzione */
                    currentThread->t_state.reg_v0 = (U32) msg->m_sender;

                    /* libero il messaggio */
                    freeMsg(msg);

                    /* Evito che rientri nel codice della syscall*/
                    currentThread->t_state.s_pc += WORD_SIZE;
                    LDST(&currentThread->t_state);
                }
            //se non è ne una send ne una recv allora attivo il trap manager predefinito, se esiste,
            //altrimenti uccido il processo e la sua progenie
            } else {
                if(currentThread->sysMgr != NULL) {
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
                } else {
                    terminate(currentThread);
                }
                //currentThread->t_state.s_pc += WORD_SIZE;
            }
        //se non è in kernel mode allora sveglio il trap manager predefinito se esiste
        //altrimenti uccido il processo e la sua progenie
        } else {
            if(currentThread->sysMgr != NULL) {
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
            } else {
                terminate(currentThread);
            }
           // currentThread->t_state.s_pc += WORD_SIZE;
        }
    } else if (cause == EXC_BREAKPOINT) {
        ;
    }
    schedule();
}

