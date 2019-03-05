#ifndef hal_avr_h
#define hal_avr_h

#include <avr/io.h>

#include "../../config/device.h"
#include "usart.h"
#include "../../macros/log.h"
#include "../api.h"

typedef struct {
    struct {
        volatile unsigned char *ddr;
        volatile unsigned char *port;
        volatile unsigned char *pin;
    } port;
    unsigned char number;
} AVRPin;

extern const HAL AVR_HAL;

#endif