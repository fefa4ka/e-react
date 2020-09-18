#include "config/device.h"

#include <Time.h>
#include <Button.h>

#include "hal/avr/api.h"

AVRPin led_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 1
};

AVRPin button_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 2
};

#define HW AVR_HAL

typedef struct
{
    rtc_datetime_t   time;
    bool             led_power;
} device_state_t;

device_state_t state = {
    .time = {0},
    .led_power = true
};

/* Application handlers */
void led_toggle(Component *trigger) {
    state.led_power = !state.led_power;
}

int main(void) {
    // Define React components
    react_define(Time, datetime);
    react_define(IO, led);
    react_define(Button, button);


    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(HW.timer),
            .time = &state.time,
        } to (datetime);

        react (Button) {
            .io = &(HW.io),
            .pin = &button_pin_hw,
            .type = toggle,
            .time = &state.time,
            .bounce_delay_ms = 100,
            .onToggle = led_toggle
        } to (button);

        React (IO) {
            .io = &(HW.io),
            .pin = &led_pin_hw,
            .mode = output,
            .level = state.led_power 
                ? high
                : low
        } to (led);
    }

    return 0;
}


