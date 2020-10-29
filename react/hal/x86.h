#pragma once

#include "api.h"
typedef struct
{
    char  *port;
    short number;
} pin_t;

extern HAL hw;

#define debug(port, pin)                                                      \
    ({                                                                        \
        pin_t debug_pin = hw_pin (port, pin);                                 \
        hw.io.out (&debug_pin);                                               \
        hw.io.flip (&debug_pin);                                              \
        hw.io.flip (&debug_pin);                                              \
    })

#define hw_pin(port, pin)                                                     \
    {                                                                         \
        #port, pin                                                            \
    }
