#pragma once

#include <stdbool.h>

struct callback {
    void (*method)(void *trigger, void *argument);
    void *argument;
};

typedef struct {
    bool (*is_available)(void *trigger);
    void (*mount)(void *trigger, void(*callback)(void *args), void *args);
    void (*umount)(void *trigger);
    void (*enable)(void *trigger, void *args);
    void (*disable)(void *trigger);
} isr_handler;

typedef struct {
    void (*in)(void *pin);
    void (*out)(void *pin);
    void (*on)(void *pin);
    void (*off)(void *pin);
    void (*flip)(void *pin);
    void (*pullup)(void *pin);
    bool (*get)(void *pin);

    isr_handler *isr;
} io_handler;

enum communication_mode {
    COMMUNICATION_MODE_TRANSCIEVER,
    COMMUNICATION_MODE_RECEIVER
};

enum pin_mode {
    PIN_MODE_OFF,
    PIN_MODE_OUTPUT,
    PIN_MODE_INPUT,
    PIN_MODE_PULLUP
};

typedef struct {
    void (*mount)(void *prescaler);
    void (*selectChannel)(void *channel);
    void (*startConvertion)(void *channel);
    bool (*isConvertionReady)(void *channel);
    int  (*readConvertion)(void *channel);

    isr_handler *isr;
} adc_handler;

typedef struct {
    void          (*init)(void *baudrate);
    bool          (*isDataReceived)();
    bool          (*isTransmitReady)();
    void          (*transmit)(unsigned char data);
    unsigned char (*receive)();

    isr_handler isr;
} serial_handler;

typedef struct {
    void         (*init)(void *config);
    unsigned int (*get)();
    void         (*set)(unsigned int ticks, void(*callback)(void *args), void *args);
    void         (*off)();
    unsigned int (*usFromTicks)(unsigned int ticks);

    isr_handler *isr;
} timer_handler;

typedef struct
{
    io_handler     io;
    adc_handler    adc;
    serial_handler uart;
    serial_handler spi;
    timer_handler  timer;
} HAL;
