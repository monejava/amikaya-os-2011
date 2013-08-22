#ifndef SSI_H
#define SSI_H

#include "const11.h"
#include "types11.h"

void terminate(tcb_t *killed);

void SSIRequest(U32 service, U32 payload, U32 *reply);
extern void SSI_function_entry_point();
struct list_head* select_io_queue_from_status_addr(memaddr status_addr);


#endif
