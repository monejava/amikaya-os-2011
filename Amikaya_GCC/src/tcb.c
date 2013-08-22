#include <tcb.e>
#include <msg.e>
#include <utils.h>

/* TCB */
HIDDEN tcb_t tcbTable[MAXTHREADS];
HIDDEN tcb_t tcbFree_h;
tcb_t* sborro[20];

/* TCB handling functions */

/* inizializza tutti i tcb_t contenuti in tcbTable mettendoli nella lista tcbFree_h,
   rendedoli di fatto tutti disponibili all'utilizzo, questa funzione è chiamata
   solo una volta durante l'inizializzazione delle strutture. */
void initTcbs(void) {
    int i;
    INIT_LIST_HEAD(&tcbFree_h.t_next);
    for (i = 0; i < MAXTHREADS; i++) {
        INIT_LIST_HEAD(&(tcbTable[i].t_inbox));
        freeTcb(&tcbTable[i]);
    }
}

/* Aggiunge il tcb_t puntato da p alla lista tcbFree_h, rendendolo quindi nuovamente disponibile*/
void freeTcb(tcb_t *p) {
    msg_t*m=NULL;
    while((m=popMessage(&(p->t_inbox), NULL)) != NULL)
        freeMsg(m);
    list_add(&p->t_next, &tcbFree_h.t_next);
}

/* Se disponibile toglie un'elemento da tcbFree_h, lo inizializza ovvero ne azzera tutti i
   campi, e ne restituisce il puntatore. Altrimenti restituisce NULL*/
/* Se disponibile toglie un'elemento da tcbFree_h, lo inizializza ovvero ne azzera tutti i
   campi, e ne restituisce il puntatore. Altrimenti restituisce NULL*/
tcb_t *allocTcb(void) {
    int i;

    tcb_t* tcb = container_of(list_next(&tcbFree_h.t_next), typeof(tcb_t), t_next);

    if (tcb != NULL) {
        /* Inizializzo le strutture che permettono di concatenare tra loro
           i tcb_t */
        list_del(&tcb->t_next);
        
        INIT_LIST_HEAD(&tcb->t_next);
        INIT_LIST_HEAD(&tcb->t_child);
        INIT_LIST_HEAD(&tcb->t_inbox);
        INIT_LIST_HEAD(&tcb->t_sib);

        tcb->t_state.s_pc = 0;
        tcb->t_state.cause = 0;        
        tcb->t_state.status = 0;
        tcb->t_state.entry_hi = 0;
        tcb->startTime = 0;
        tcb->elapsedTime = 0;
        tcb->executionTime = 0;
        tcb->prgMgr = NULL;
        tcb->sysMgr = NULL;
        tcb->tlbMgr = NULL;
        tcb->t_parent = NULL;
        tcb->waitingReply = FALSE;
        
        for (i = 0; i < MAXREG; i++) {
            tcb->t_state.gpr[i] = 0;
        }
    }
    return tcb;
}

/* Inizializza il list_head puntato da emptyList il quale sarà il puntatore alla
   testa di una coda di tcb_t*/
void mkEmptyThreadQ(struct list_head *emptylist) {
    INIT_LIST_HEAD(emptylist);
}

/* Restituisce TRUE se la coda di tcb_t puntata da head è vuota, altrimenti FALSE*/
int emptyThreadQ(struct list_head *head) {
    return list_empty(head);
}

/* Se il thread puntato da p è contenuto nella lista puntata da head restituisce TRUE altrimenti FALSE*/
int containsThread(struct list_head *head, tcb_t *p) {
    struct list_head *pos;

    /*Scorro tutti gli elementi della coda puntata da head per vedere se p
      appartiene ad essa*/
    list_for_each(pos, head) {
        if (pos == &p->t_next) {
            return TRUE;
        }
    }
    
    return FALSE;
}

/* Inserisce il tcb_t puntato da p alla fine della coda la cui testa è puntata da head*/
void insertThread(struct list_head *head, tcb_t *p) {
    list_add_tail(&p->t_next, head);
}

/* Rimuove il primo elemento della coda la cui testa è puntata da head, restituisce
   NULL se la coda è vuota*/
tcb_t *removeThread(struct list_head *head) {
    tcb_t* tcb;
    if(list_empty(head))
        return NULL;
    
    tcb = container_of(list_next(head), typeof(tcb_t), t_next);
    list_del(&tcb->t_next);
    return tcb;
}

/* Rimuove il tcb_t puntato da p dalla coda la cui testa è puntata da head.
   Se l'elemento p non è presente nella coda allora restituisce NULL*/
tcb_t *outThread(struct list_head *head, tcb_t *p) {
    struct list_head *t_temp = NULL;

    /* se il puntatore a coda */
    if(emptyThreadQ(head))
        return NULL;

    /* cerco in tutta la lista se c'è una corrispondenza con con p */
    list_for_each(t_temp, head) {
        if( t_temp == &(p->t_next) ) {/* nel caso la trovo allora la elimino e lo restituisco */
            list_del( &(p->t_next) );
            INIT_LIST_HEAD(&(p->t_next));
            return p;
        }
    }
    /* altrimenti restituisco NULL */
    return NULL;
}

/* Restituisce il primo tcb_t della coda la cui testa è puntata da head.
   Il tcb_t non viene però rimosso dalla coda. Restituisce NULL la coda
   è vuota*/
tcb_t *headThread(struct list_head *head) {
    return container_of(list_next(head), typeof(tcb_t), t_next);
}

/* Tree view functions */

/* Restituisce TRUE se il il tcb_t puntato da parent non ha filgi, altrimenti FALSE*/
int emptyChild(tcb_t *parent) {
    return list_empty(&parent->t_child);
}

/* Rende il tcb_t puntato da child figlio del tcb_t puntato da parent*/
void insertChild(tcb_t *parent, tcb_t *child) {
    list_add(&child->t_sib, &parent->t_child);
    child->t_parent = parent;
}

/* Toglie il primo figlio del tcb_t puntato da parent e ne restituisce il puntatore
   rendendolo di fatto non più figlio di parent. Restituisce NULL se parent non ha
   figli */
tcb_t *removeChild(tcb_t *parent)
{
	if(emptyChild(parent)) return NULL;
	else{
		tcb_t *child = container_of(parent->t_child.next, struct tcb_t, t_sib);
		list_del(&(child->t_sib));
		child->t_parent = NULL;
		return child;
	}
}

/* Rende il tcb_t puntato da child non più figlio del suo genitore. Se il figlio
   puntato da child na ha genitore, oppure è il primo figlio del suo genitore
   allora restutuisce NULL*/
tcb_t *outChild(tcb_t *child) {
    if (child->t_parent == NULL) {
        return NULL;
    } else {
        list_del(&child->t_sib);
        child->t_parent = NULL;
    }
    return child;
}
