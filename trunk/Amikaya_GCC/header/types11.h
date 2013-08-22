#ifndef _TYPES11_H
#define _TYPES11_H

#include "uMPS_include/uMPStypes.h"
#include "listx.h"


typedef unsigned int cpu_t;

typedef struct tcb_t {
    /*process queue fields*/
    struct list_head    t_next;

    /*process tree fields*/
    struct tcb_t        *t_parent;
    struct list_head    t_child,
                        t_sib;

    /*processor state, etc*/
    state_t             t_state;

    /*msg queue */
    struct list_head    t_inbox;

    /*usata per sapere quanto è rimasto in esecuzione il processo*/
    cpu_t               startTime; /* Inizio dell'esecuzione*/
    cpu_t               elapsedTime; /*tempo passato da inizio esecuzione*/
    cpu_t               executionTime; /* tempo totale esecuzione*/
    U32                 waitingReply; /* se il thread e' in attesa di una reply */
    
    struct tcb_t	*prgMgr, 
                        *sysMgr, 
                        *tlbMgr;
} tcb_t;


typedef struct payload_t{ //from ssi to ssi
    U32 exist;
    U32 payload;
} payload_t;

typedef struct device_t{ //from ssi to ssi
    payload_t disk[8];
    payload_t tape[8];
    payload_t ethe[8];
    payload_t prin[8];
    payload_t term[8];    
} device_t;

typedef struct ssimsg_t{ //from ssi to ssi
    U32 service;
    U32 payload;
    tcb_t* sender;    
    U32 reply;
} ssimsg_t;

typedef struct {
    /* msg queue */
    struct list_head    m_next;

    /* thread that sent this message */
    struct tcb_t        *m_sender;
    
    ssimsg_t            msg_ssi;

    /* payload */
    S32                 m_message;
} msg_t;

#endif
