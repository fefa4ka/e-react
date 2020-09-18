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
} AVRPin;

HAL AVR_HAL;

#endif
