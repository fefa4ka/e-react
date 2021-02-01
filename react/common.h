#pragma once

#include <stddef.h>
#include <stdio.h>

enum eLogSubSystem {
    common,
    react
};


enum eError {
    eErrorNone = 0,
    eErrorBufferFull,
    eErrorBufferEmpty,
    eErrorBufferBusy
};

typedef struct 
{
    unsigned long quot;
    unsigned short rem;
} divmod10_t;

char * utoa(unsigned long value);
char * itoa(long value);
inline static void divmodu10(divmod10_t *res,unsigned long n);


#define bit_value(data, bit) ((data >> bit) & 1) /** Return Data.Y value **/
#define bit_set(data, bit)   data |= (1 << bit)  /** Set Data.Y   to 1    **/
#define bit_clear(data, bit) data &= ~(1 << bit) /** Clear Data.Y to 0    **/
