#ifndef utils_common_h
#define utils_common_h

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


char * utoa(unsigned long value);
char * itoa(long value);

#endif
