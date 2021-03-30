#pragma once

//#include <stddef.h>
//#include <stdio.h>

typedef struct {
    unsigned long  quot;
    unsigned short rem;
} divmod10_t;

char *             utoa (unsigned long value);
char *             itoa (long value);
extern inline int  random ();
unsigned char      reverse (unsigned char b);

