#pragma once

#include <stdint.h>
#include <stdbool.h>

struct callback {
    void (*method)(void *trigger, void *argument);
    void *argument;
};

typedef struct {
    bool (*is_available)(void *trigger);
    void (*mount)(void *trigger, void (*callback)(void *args), void *args);
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
    int16_t (*readConvertion)(void *channel);

    isr_handler *isr;
} adc_handler;

typedef struct {
    void (*init)(void *baudrate);
    bool (*isDataReceived)();
    bool (*isTransmitReady)();
    void (*transmit)(unsigned char data);
    unsigned char (*receive)();

    isr_handler isr;
} serial_handler;

typedef struct {
    void (*init)(void *config);
    uint16_t (*get)();
    void (*set)(uint16_t ticks, void (*callback)(void *args), void *args);
    void (*off)();
    uint16_t (*usFromTicks)(uint16_t ticks);

    isr_handler *isr;
} timer_handler;

typedef struct {
    io_handler     io;
    adc_handler    adc;
    serial_handler uart;
    serial_handler spi;
    timer_handler  timer;
} HAL;

#ifdef REACT_PROFILER
struct HAL_calls {
    uint64_t gpio_in;
    uint64_t gpio_out;
    uint64_t gpio_on;
    uint64_t gpio_off;
    uint64_t gpio_flip;
    uint64_t gpio_pullup;
    uint64_t gpio_get;

    uint64_t adc_mount;
    uint64_t adc_selectChannel;
    uint64_t adc_startConvertion;
    uint64_t adc_isConvertionReady;
    uint64_t adc_readConvertion;

    uint64_t uart_init;
    uint64_t uart_isDataReceived;
    uint64_t uart_isTransmitReady;
    uint64_t uart_transmit;
    uint64_t uart_receive;

    uint64_t timer_init;
    uint64_t timer_get;
    uint64_t timer_set;
    uint64_t timer_off;
    uint64_t timer_usFromTicks;
};
#endif

