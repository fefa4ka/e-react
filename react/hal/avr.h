#pragma once

#include <avr/interrupt.h>
#include <avr/io.h>

#include "api.h"

typedef struct
{
    struct
    {
        volatile unsigned char *ddr;
        volatile unsigned char *port;
        volatile unsigned char *pin;
    } port;
    unsigned char number;
} pin_t;

HAL hw;

#define debug(port, pin)                                                      \
    ({                                                                        \
        pin_t debug_pin = hw_pin (port, pin);                                 \
        hw.io.out (&debug_pin);                                               \
        hw.io.flip (&debug_pin);                                              \
        hw.io.flip (&debug_pin);                                              \
    })

#define hw_pin(port, pin)                                                     \
    {                                                                         \
        { &PORT##port, &DDR##port, &PIN##port }, pin                          \
    }
#define hw_uart_baudrate(baudrate)                                            \
    (((F_CPU) + 4UL * (baudrate)) / (8UL * (baudrate)) - 1UL)
