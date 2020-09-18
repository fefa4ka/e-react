#include "config/device.h"

#ifdef ARCH_AVR
#include "hal/avr/api.h"
AVRPin Debug_Pin_HW = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 1
};
#endif

#include <IO.h>

typedef struct
{
    bool             debug;
} DeviceState;

DeviceState State = {
    .debug = true
};

int main(void) {
    // Define React components
    Define(IO, Debug_Pin);

    // Event-loop
    while (true) { 
        // Debug Step
        State.debug = !State.debug;

        React(IO) {
            .io = &(AVR_HAL.io),
            .pin = &Debug_Pin_HW,
            .mode = output,
            .level = State.debug, 
        } to(Debug_Pin);
    }

    return 0;
}

