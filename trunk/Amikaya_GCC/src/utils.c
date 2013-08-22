#include "listx.h"
#include "types11.h"
#include "utils.h"
#include "const11.h"
#include "boot.e"
#include "libumps.e"
#include "p2test.0.2.e"
#include <scheduler.e>


//#define STAMPA

typedef unsigned int devreg;

char ___out2[2400];
char unknown[11];
char* OUTPUT    = ___out2;
U32 indexMatr=0;



/* serve come breakpoint alla fine della stampa di una stringa */
void aaaaaBP_PRINTFK(void){}

/* questa funzione è collegata alla printOnMemory().
 * in base all'index di che riga stiamo scrivendo calcola quale sia la riga dopo e la cancella */
void resetLine(){
    U32 p=((indexMatr+1)%20);
    char*tmp=OUTPUT+(120*p);
    fillmem(tmp,0, 120);
}
/* funzione che emula il terminale:
 * in fase di debugging è stata utile perchè prendendo un'area molto vasta di memoria,
 * e analizzandola col tracer di umps2 riusciamo a visualizzare quello che ci scriviamo.
 * questa funzione, in particolare, permette ad ogni chiamata di andare a scrivere ad una riga diversa
 * cosicche le vecchie stampe non vadano sovrascritte. 
 * come lunghezza della riga abbiamo preso 120 caratteri, 
 * indexMatr è l'indice su che riga dobbiamo scrivere. */
void printOnMemory(char* s)
{
    char*tmp=OUTPUT+(120*indexMatr);
    indexMatr %= 20;
    while((*(tmp++) = *(s++))!=0);
    indexMatr++;
    resetLine();
    aaaaaBP_PRINTFK();
}

/* copia 2 aree di memoria */
void* memcopy(void *destination, const void *source, U32 size) {
    char* dest = (char*) destination;
    char* src = (char*) source;

    while (size--)
        *(dest++) = *(src++);

    return dest;
}

/* riempie un'area di memoria lunga size con value */
void* fillmem(void *destination, char value, U32 size) {
    int _size=0;
    char* dest = (char*)destination;

    while (_size++ < size)
        *(dest++) = value;

    return destination;
}
/* restituisce la lunghezza della stringa */
U32 _strlen(char* s){
    U32 count=0;
    while (*s++) 
        count++;
    return count;
}
/* copia la stringa src in dest e restituisce il numero di caratteri copiati */
int __strcpy(char *dest, const char *src) {
    int count = 0;
    while ((*(dest++) = *(src++))!=0)
        count++;
    return count;
}
/* se c è un carattere minuscolo lo converte in maiuscolo */
char __toupper(char c) {
    if (c >= 'a' && c <= 'z')
        return (c - 32);
    return c;
}
/* utile per convertire una stringa in maiuscolo */
void convertToUpperCase(char *sPtr) {
    int i = 0;
    for (i = 0; i < 11; i++)
        sPtr[i] = __toupper((unsigned char) sPtr[i]);
}
/* funzione della lib C per trasformare un numero in stringa (integer to ascii) */
char *_itoa(int value, char *string, int radix) {
    char tmp[33];
    char *tp = tmp;
    int i;
    unsigned v;
    int sign;
    char *sp;

    if (radix > 36 || radix <= 1) {
        return 0;
    }
    sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (unsigned) value;
    while (v || tp == tmp) {
        i = v % radix;
        v = v / radix;
        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }
    sp = string;
    if (sign)
        *sp++ = '-';
    while (tp > tmp)
        *sp++ = *--tp;
    *sp = 0;
    return string;
}

/* fase di debugging:
 * in base all'indirizzo di memoria capiamo di che threadBlock si tratta
 * e restituiamo la stringa con il suo nome */
char* threadName(tcb_t* t) {
    if (t == tcb_SSI)      return "tcb_SSI";
    else if (t == tcb_test)     return "tcb_test";
    else if (t == tcb_init)     return "tcb_init";
    else if (t == NULL)         return "(NULL)";
    else if (t == tcb_1)        return "tcb_asd1";
    else if (t == tcb_2)        return "tcb_asd2";
    else if (t == tcb_3)        return "tcb_asd3";
    else if (t == printid)      return "printid";
    else if (t == p2id)         return "p2id";
    else if (t == p3id)         return "p3id";
    else if (t == p4id)         return "p4id";
    else if (t == p5id)         return "p5id";
    else if (t == p6id)         return "p6id";
    else if (t == p7id)         return "p7id";
    else if (t == p8id)         return "p8id";
    else if (t == p9id)         return "p9id";    
    else if ((U32)t == SSI_MAGIC)       return "SSI_MAGIC";
    else if ((U32)t == TERMINAL0ADDR)   return "TERMINAL0ADDR";
    else if ((U32)t == 0x10000258)      return "STATUS_TRANSMIT_T0";
    
    else {
        _itoa((U32)t, unknown, 16);
        return unknown;
    }
}
/* fase di debugging:
 * in base all'indirizzo di memoria restituiamo la stringa con il nome della lista */
char* lhName(struct list_head* l) {
    if (l == NULL)                      return "(NULL)";
    else if (l == &readyQueue)          return "ReadyQ";
    else if (l == &waitQueue)           return "WaitQ";
    else if (l == &waitForPseudoClockQueue)     return "WFSCQ";
    else {
        _itoa((U32)l, unknown, 16);
        return unknown;
    }    
}
/* fase di debugging:
 * in base al servizio inviato restituiamo la stringa con il nome del servizio */
char* serviceName(int service) {

    switch(service)
    {
        case CREATEBROTHER :    return "CREATEBROTHER";
        case CREATESON :        return "CREATESON";
        case TERMINATE :        return "TERMINATE";
        case SPECPRGMGR :       return "SPECPRGMGR";
        case SPECTLBMGR :       return "SPECTLBMGR";
        case SPECSYSMGR :       return "SPECSYSMGR";
        case GETCPUTIME :       return "GETCPUTIME";
        case WAITFORCLOCK :     return "WAITFORCLOCK";
        case WAITFORIO :        return "WAITFORIO";
        case WAKE_UP_PSEUDO_CLOCK : return "WAKE_UP_PSEUDO_CLOCK";
        case WAKE_UP_FROM_IO:   return "WAKE_UP_FROM_IO";
        default :               return "UNKNOWN";
    }
    
}
/* versione della printf per stampare in memoria.
 * prende in ingresso il formato che deve avere la stringa e n argomenti.
 * fa il parsing della stringa _fmt e cerca il flag % per cercare cosa stampare. */
int ker_printf(char *_fmt, ...) {   

    int pc = 0;
    char sNumber[11];
    char ___out[200];
    char* format = _fmt;
    va_list ptr;

    va_start(ptr, _fmt);

    for (; *format != 0; ++format) {
        if (*format == '%') {
            ++format;
            switch(*format)
            {
                case '\0': break;
                case '%': goto out;
                case 's': {
                    char* s = va_arg(ptr, char*);
                    pc += __strcpy(&(___out[pc]), s ? s : "(null)");
                    break;
                }
                case 'd': {
                    pc += __strcpy(&(___out[pc]), _itoa((U32) va_arg(ptr, U32), sNumber, 10));
                    break;
                }
                case 'b': {
                    pc += __strcpy(&(___out[pc]), _itoa((U32) va_arg(ptr, U32), sNumber, 2));
                    break;
                }
                case 'x': {
                    pc += __strcpy(&(___out[pc]), _itoa((U32) va_arg(ptr, U32), sNumber, 16));
                    break;
                }
                case 'X': {
                    char sNumber[30];
                    _itoa((U32) va_arg(ptr, U32), sNumber, 16);
                    convertToUpperCase(sNumber);

                    pc += __strcpy(&(___out[pc]), sNumber);
                    break;
                }
                case 'c': {
                    ___out[pc++] = va_arg(ptr, int);
                    break;
                }
            }
        } else {
out:
            ___out[pc++] = *format;
        }
    }
    if (___out[pc]) ___out[pc] = '\0';

    va_end(ptr);
    
    printOnMemory(___out);

    return pc;
}
