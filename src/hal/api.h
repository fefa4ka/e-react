#ifndef hal_api_h
#define hal_api_h

#include "../macros/types.h"
//#include <stdio.h>

typedef struct {
    void (*in)(void *pin);
    void (*out)(void *pin);
    void (*on)(void *pin);
    void (*off)(void *pin);
    void (*flip)(void *pin);
    void (*pullup)(void *pin);
    bool (*get)(void *pin);
} io_handler;

typedef struct {
    void              (*mount)(void *prescaler);
    void              (*selectChannel)(void *channel);
    void              (*startConvertion)(void *channel);
    bool              (*isConvertionReady)(void *channel);
    int               (*readConvertion)(void *channel);
} adc_handler;

typedef struct {
    void (*init)(void *baudrate);
    bool (*isDataReceived)();
    bool (*isTransmitReady)();
    void (*transmit)(unsigned char data);
    unsigned char (*receive)();
} uart_handler;

typedef struct {
    void (*init)(void *config);
    unsigned int (*getTimer)();
    unsigned int (*usFromTicks)(unsigned int ticks);
} timer_handler;

typedef struct
{
    io_handler    io;
    adc_handler   adc;
    uart_handler  uart;
    timer_handler timer;
} HAL;

#endif
