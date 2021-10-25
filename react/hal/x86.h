#pragma once

#include "../hash.h"
#include "api.h"
#include <component.h>

typedef struct {
    unsigned char number;
    char *        name;
    char          index;
    struct {
        unsigned char ddr;
        unsigned char port;
        unsigned char pin;
    } port;
} pin_t;

extern HAL hw;

void free_pins();
void dump_pin(pin_t *pin);
void gpio_init();
void log_init();
void log_clean();
void vcd_init();
void vcd_clean();
unsigned long time_in_ns();
unsigned long timer_get_ns();
const char *int_to_binary_str(int x, int N_bits);
void log_printf(char *message, ...);

#define test(...) test_program({ gpio_init(); }, { free_pins(); }, __VA_ARGS__)

#define hw_isr_enable()                                                        \
    {                                                                          \
    }
#define hw_isr_disable()                                                       \
    {                                                                          \
    }
#define debug(port, pin)                                                       \
    ({                                                                         \
        pin_t debug_pin = hw_pin(port, pin);                                   \
        hw.io.out(&debug_pin);                                                 \
        hw.io.flip(&debug_pin);                                                \
        hw.io.flip(&debug_pin);                                                \
    })

#define hw_pin(port, pin)                                                      \
    {                                                                          \
        pin, #port, { 0 }                                                      \
    }
