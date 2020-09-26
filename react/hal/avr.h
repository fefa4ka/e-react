#pragma once 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "api.h"

typedef struct {
    struct {
        volatile unsigned char *ddr;
        volatile unsigned char *port;
        volatile unsigned char *pin;
    } port;
    unsigned char number;
} pin_t;

HAL hw;

#define debug(port, pin) \
    ({ \
         hw_pin(debug, port, pin); \
         hw->io->flip(debug); \
         hw->io->flip(debug); \
    }) \

#define hw_pin(port, pin) { { &PORT##port, &DDR##port, &PIN##port }, pin }
#define hw_uart_baudrate(baudrate) (((F_CPU) + 4UL * (baudrate)) / (8UL * (baudrate)) - 1UL)


