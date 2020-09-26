#include "config.h"

#define BUFFER_SIZE 128 
unsigned char output_buffer[BUFFER_SIZE];

#define BUFFER_SIZE 128 
unsigned char input_buffer[BUFFER_SIZE];

struct device state = {
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
    react_define(Button, button);
    react_define(Bitbang, spi);

    void spi_hw[] = { &mosi_pin_hw, &miso_pin_hw, NULL };
    //enum pin_mode_e spi_modes[] = { output, input, NULL };

    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(hw.timer),
            .time = &state.time,
        } to (datetime);

        react (Button) {
            .io = &(hw.io),
            .pin = &state.signal_pin,
            .type = toggle,
            .time = &state.time,
            .bounce_delay_ms = 100,
            .onToggle = button_toggle
        } to (button);

        react (Bitbang) {
            .io = &(hw.io),
            .time = &state.time,
            .baudrate = 9600,
            .pins = { &state.mosi_pin, &state.miso_pin },
            .clock = &state.clk_pin,
            .modes = { input, output },
            .buffers = &state.serial_buffer
        } to (spi);
    }

    return 0;
}


