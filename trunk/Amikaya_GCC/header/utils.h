#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdarg.h>
#include "uMPS_include/const.h"


void* memcopy(void *destination, const void *source, U32 size);
#define copyRegState(destination, source) memcopy((void *)destination, (void*)source, sizeof(state_t))


/* debugging function */

void printOnMemory(char* s);
char* serviceName(int service);
char* threadName(tcb_t* t);
char* lhName(struct list_head* l);
void* fillmem(void *destination, char value, U32 size);
char *_itoa(int value, char *string, int radix);
int ker_printf(char *format, ...);


/* uso questa define per evitare un interrupt durante la scrittura
 * infatti subito prima di richiamare la ker_printf mi salvo quanti ms
 * ha ancora a disposizione il currentThread e imposto il valore del timeslice altissimo
 * così ho la sicurezza di finire la stampa entro il prossimo interrupt del timer.
 * quando ho finito la stampa riimposto quanti ms rimanevano al current thread. */
#define printfk(format, ...)                                    \
        do{  U32 __TIME_START = *((U32 *)BUS_INTERVALTIMER);    \
             SET_IT(50000);                                     \
             ker_printf(format, ##__VA_ARGS__);                 \
             SET_IT(__TIME_START+5);                            \
        }while(0)
#endif
