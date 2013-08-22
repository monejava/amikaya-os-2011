#ifndef TCB_E
#define TCB_E
#include "uMPS_include/const.h"
#include "types11.h"
#include "const11.h"
#include "listx.h"

/* TCB handling functions */

void freeTcb(tcb_t *p);
tcb_t *allocTcb(void);
void initTcbs(void);

/* Queue view functions */
void mkEmptyThreadQ(struct list_head *emptylist);
int emptyThreadQ(struct list_head *head);
int containsThread(struct list_head *head, tcb_t *p);
void insertThread(struct list_head *head, tcb_t *p);
tcb_t *removeThread(struct list_head *head);
tcb_t *outThread(struct list_head *head, tcb_t *p);
tcb_t *headThread(struct list_head *head);

/* Tree view functions */
int emptyChild(tcb_t *this);
void insertChild(tcb_t *parent, tcb_t *child);
tcb_t *removeChild(tcb_t *parent);
tcb_t *outChild(tcb_t *child);






#endif
