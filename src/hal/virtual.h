#ifndef hal_virtual_h
#define hal_virtual_h

#include "api.h"

typedef struct {
    char port;
    short number;
} VirtualPin;

extern const HAL Virtual_HAL;

#endif