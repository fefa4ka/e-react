#include "config/device.h"

#include <Calendar.h>
#include <Button.h>
#include <Bitbang.h>

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

//define_pin(miso_pin_hw, B, 3);
//AVRPin miso_pin_hw = { { &PORTB, &DDRB, &PINB }, 3 };
AVRPin miso_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 3
};

AVRPin mosi_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 4
};

AVRPin clk_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 5
};

#define HW AVR_HAL

#define BUFFER_SIZE 128 
unsigned char output_buffer[BUFFER_SIZE];

#define BUFFER_SIZE 128 
unsigned char input_buffer[BUFFER_SIZE];

typedef struct
{
    rtc_datetime_t   time;
    bool             led_power;
    struct ring_buffer_s 
                      output_buffer;
    struct ring_buffer_s 
                      input_buffer;
} device_state_t;

device_state_t state = {
    .time = {0},
    .led_power = true,
    .output_buffer = { 
        .size = BUFFER_SIZE,
        .data = output_buffer
    },
    .input_buffer = { 
        .size = BUFFER_SIZE,
        .data = input_buffer
    },
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
    react_define(Bitbang, spi);

    void spi_hw[] = { &mosi_pin_hw, &miso_pin_hw, NULL };
    //enum pin_mode_e spi_modes[] = { output, input, NULL };

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
            .onToggle = button_toggle
        } to (button);

        react (Bitbang) {
            .io = &(HW.io),
            .time = &state.time,
            .baudrate = 9600,
            .pins = spi_hw,
            .clock = &clk_pin_hw,
            .modes = { input, output },
            .buffers = &state.serial_buffer
        } to (spi);

        React (IO) {
            .io = &(HW.io),
            .pin = &led_pin_hw,
            .mode = { output, input },
            .buffers = { &output_buffer, input_buffer },
            .level = state.led_power 
                ? high
                : low
        } to (led);
    }

    return 0;
}


