#ifndef MSG_E
#define MSG_E

#include "tcb.e"
#include "uMPS_include/const.h"
#include "types11.h"
#include "listx.h"

/* Message list handling functions */

void initMsg(void);
void freeMsg(msg_t *m);
msg_t *allocMsg(void);

void mkEmptyMessageQ(struct list_head *emptylist);
int emptyMessageQ(struct list_head *head);
void insertMessage(struct list_head *head, msg_t *mp);
void pushMessage(struct list_head *head, msg_t *mp);
msg_t *popMessage(struct list_head *head, tcb_t *mp);
msg_t *headMessage(struct list_head *head);




#endif
