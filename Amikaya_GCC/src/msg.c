#include <msg.e>

#include <utils.h>

/* Message */
HIDDEN msg_t msgTable[MAXMESSAGES];
HIDDEN msg_t msgFree_h;

//#define DBG_MSG    

/* Message list handling functions */

/* Inizializza tutti i msg_t contenuti in msgTable mettendoli nella lista msgFree_h,
   rendedoli di fatto tutti disponibili all'utilizzo, questa funzione è chiamata
   solo una volta durante l'inizializzazione delle strutture. */
void initMsg(void) {
    int i;
    INIT_LIST_HEAD(&msgFree_h.m_next);
    for (i = 0; i < MAXMESSAGES; i++) {
        freeMsg(&msgTable[i]);
    }
}

/* Aggiunge l'msg_t puntato da m alla lista msgFree_h, rendendolo quindi nuovamente disponibile*/
void freeMsg(msg_t *m) {
    list_add(&m->m_next, &msgFree_h.m_next);
}

/* Se disponibile toglie un'elemento da msgFree_h, lo inizializza ovvero ne azzera tutti i
   campi, e ne restituisce il puntatore. Altrimenti restituisce NULL*/
msg_t *allocMsg(void){
    msg_t * m_temp = NULL;

    /* se la msgFree è vuota(quindi non ci sono piu msg_t disponibili) restituisce NULL */
    if(list_empty(&(msgFree_h.m_next)))
        return NULL;

    /* faccio puntare m_temp al prossimo elemento della lista */
    m_temp = (msg_t *)container_of(msgFree_h.m_next.next, typeof(msg_t), m_next);

    /* rimuovo l'elemento che verrà restituito dalla msgFree_h */
    list_del(&(m_temp->m_next));

    /* inizializzo ogni parametro */
    m_temp->m_message = 0;
    m_temp->m_sender = NULL;
    INIT_LIST_HEAD( &(m_temp->m_next) );

    return m_temp;
}

/* Inizializza il list_head puntato da emptyList il quale sarà il puntatore alla
   testa di una coda di msg_t*/
void mkEmptyMessageQ(struct list_head *emptylist) {
    INIT_LIST_HEAD(emptylist);
}

/* Restituisce TRUE se la coda di msg_t puntata da head è vuota, altrimenti FALSE*/
int emptyMessageQ(struct list_head *head) {
    return list_empty(head);
}

/* Inserisce il tcb_t puntato da mp alla fine della coda la cui testa è puntata da head*/
void insertMessage(struct list_head *head, msg_t *mp) {
    list_add_tail(&(mp->m_next), head);
}

/*  Inserisce il tcb_t puntato da mp all'inizio della coda la cui testa è puntata da head*/
void pushMessage(struct list_head *head, msg_t *mp) {
    list_add(&mp->m_next, head);
}

/* Rimuove il primo msg_t, dalla coda la cui testa è puntata da head, il quale
   sender è t_ptr è ne restituisce il puntatore. Se il parametro t_ptr passato è
   NULL allora restituisco il primo msg_t della coda puntata da head. Restituisce
   NULL se non ci sono msg_t per t_ptr oppure se la coda è vuota*/
msg_t *popMessage(struct list_head *head, tcb_t *t_ptr) {
    msg_t *msg=NULL;
    struct list_head *pos=NULL;

    if (emptyMessageQ(head))
        return NULL;

    /* se mp = NULL
     *     e se la lista head è vuota allora restituisco NULL;
     *     se invece la lista head non è vuota trovo il primo Messaggio nella lista, lo elimino,
     *     aggiorno m_sender e lo restituisco */
    if(t_ptr == NULL) {
        if (emptyMessageQ(head))
            return NULL;
        msg = container_of(head->next, typeof(msg_t), m_next);
        list_del(head->next);
        return msg;
    }

    /* Scorro tutti gli elementi della coda puntata da head in cerca di un msg_t
       il cui sender sia t_ptr*/
    list_for_each(pos, head) {
        msg = container_of(pos, typeof(msg_t), m_next);
        if (msg->m_sender == t_ptr) {
            list_del(pos);
            return msg;
        }
    }
    return NULL;
}

/* Restituisce il primo msg_t della coda la cui testa è puntata da head.
   Il msg_t non viene però rimosso dalla coda. Restituisce NULL la coda
   è vuota*/
msg_t *headMessage(struct list_head *head) {
    return container_of(list_next(head), typeof(msg_t), m_next);
}


