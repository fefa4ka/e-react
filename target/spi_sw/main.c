#include "config.h"

unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device state = {
    .time = {0},
    .signal_pin = hw_pin(B, 2),
    .clk_pin = hw_pin(B, 1),
    .mosi_pin = hw_pin(B, 3),
    .miso_pin = hw_pin(B, 4),
    .input_buffer = { input_buffer, BUFFER_SIZE },
    .output_buffer = { output_buffer, BUFFER_SIZE },
    .index = 0
};

pin_t *spi_pins[] = { &state.mosi_pin, &state.miso_pin, NULL };
struct ring_buffer *spi_buffers[] = { &state.output_buffer, &state.input_buffer };
enum pin_mode spi_modes[] = { OUTPUT, INPUT };

/* Application handlers */
void send_number(Component *trigger) {
    state.index++;

    rb_write(&state.output_buffer, state.index); 
}

int main(void) {
    // Define React components
    component(Time, datetime);
    component(Button, button);
    component(Bitbang, spi);

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
            .onToggle = send_version 
        } to (button);
    
        
        react (Bitbang) {
            .io = &(hw.io),
            .time = &state.time,
            .baudrate = 9600,
            .pins = spi_pins,
            .clock = &state.clk_pin,
            .modes = spi_modes,
            .buffers = spi_buffers,

            .onTransmit = send_number 
        } to (spi);
    }

    return 0;
}


