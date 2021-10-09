#pragma once

#include <stdlib.h>
#include "api.h"
#include "../hash.h"
typedef struct
{
    unsigned char number;
    char  *name;
    struct
    {
        unsigned char ddr;
        unsigned char port;
        unsigned char pin;
    } port;
} pin_t;

extern HAL hw;

void free_pins();

#define debug(port, pin)                                                      \
    ({                                                                        \
        pin_t debug_pin = hw_pin (port, pin);                                 \
        hw.io.out (&debug_pin);                                               \
        hw.io.flip (&debug_pin);                                              \
        hw.io.flip (&debug_pin);                                              \
    })

#define hw_pin(port, pin)                                                     \
    {                                                                         \
        pin, #port, {0}                                                           \
    }
