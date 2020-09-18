#include "config/device.h"

#ifdef ARCH_AVR
#include "hal/avr/api.h"
AVRPin debug_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 1
};
#define HW AVR_HAL
#endif

#ifdef ARCH_x86
#include "hal/virtual.h"
VirtualPin Debug_Pin_HW = {
    .port = 'D',
    .number = 1
};
#define HW Virtual_HAL
#endif

#include <IO.h>

typedef struct
{
    bool             debug;
} device_state_t;

device_state_t state = {
    .debug = true
};

int main(void) {
    // Define React components
    react_define(IO, debug_pin);

    // Event-loop
    while (true) { 
        // Debug Step
        state.debug = !state.debug;

        react (IO) {
            .io = &(HW.io),
            .pin = &debug_pin_hw,
            .mode = output,
            .level = state.debug, 
        } on (debug_pin);
    }

    return 0;
}

