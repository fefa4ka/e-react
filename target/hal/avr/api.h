#ifndef hal_avr_h
#define hal_avr_h

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../config/device.h"
//#include "usart.h"
#include <hal.h>

typedef struct {
    struct {
        volatile unsigned char *ddr;
        volatile unsigned char *port;
        volatile unsigned char *pin;
    } port;
    unsigned char number;
} pin_t;

#define hw_pin(name, port, pin) pin_t name = { { &PORT##port, &DDR##port, &PIN##port }, pin }

HAL hw;

#endif
