#include "config.h"

#define signal_pin hw_pin (B, 2)

unsigned char        output_buffer[BUFFER_SIZE];
unsigned char        input_buffer[BUFFER_SIZE];
struct ring_callback *callback_buffer[BUFFER_SIZE] = {0};
pin_t                *chip_select_buffer[BUFFER_SIZE] = {0};

struct device state = {
    .time          = { 0 },
    .clk_pin       = hw_pin (D, 1),
    .mosi_pin      = hw_pin (D, 2),
    .miso_pin      = hw_pin (D, 3),
    .input_buffer  = { input_buffer, BUFFER_SIZE },
    .output_buffer = { output_buffer, BUFFER_SIZE },
    .index         = 0,
};
pin_t *             spi_pins[] = { &state.mosi_pin, &state.miso_pin, NULL };
struct ring_buffer *spi_buffers[]
    = { &state.output_buffer, &state.input_buffer };
enum pin_mode spi_modes[] = { OUTPUT, INPUT };

/* Application handlers */
void
send_number (Component *trigger)
{
    state.index++;

    rb_write (&state.output_buffer, state.index);
}

void 
write_address(unsigned char address, unsigned char value, pin_t *chip_select_pin) 
{
    rb_write(&state.output_buffer, address);
    rb_write(&state.output_buffer, value);
    chip_select_buffer[state.output_buffer.read] = chip_select_pin;
}

void
send_command_callback(unsigned char address, unsigned char value, pin_t *chip_select_pin, struct ring_callback *callback) {
    write_address(address, value, chip_select_pin);
    callback_buffer[state.output_buffer.read] = callback;
}

void
read_address(unsigned char address, pin_t *chip_select_pin, struct ring_callback *callback) {
    send_command_callback(address, 0, callback);
}

void
spi_init(Component *instance) {
    pin_t *chip_select_pin = chip_select_buffer[state.output_buffer.read];
    if(chip_select_pin) {
        hw.io.out(chip_select_pin);
        hw.io.on(chip_select_pin);
    }
}

void 
spi_receive(Component *instance) {
    pin_t *chip_select_pin = chip_select_buffer[state.output_buffer.read];
    struct ring_callback *callback = callback_buffer[state.output_buffer.read];
    unsigned char data;
    rb_read(&state.input_buffer, &data);

    if(callback) {
        callback->method(data, callback->argument);
        callback_buffer[state.output_buffer.read] = NULL;
    }
    if(chip_select_pin) {
        hw.io.off(chip_select_pin);
        chip_select_buffer[state.output_buffer.read] = NULL;
    }
}

int
main (void)
{
    // Define React components
    Time (datetime);
    Button (button);
    Bitbang (spi);

    send_number(NULL);
    // Event-loop
    while (true) {
        // Timer component, for event management and time counting
        react (Time, datetime,
               _ ({
                   .timer = &hw.timer,
                   .time  = &state.time,
               }));

        react (Button, button,
               _ ({
                   .io              = &hw.io,
                   .pin             = hw_pin (B, 2),
                   .type            = BTN_PUSH_PULLUP,
                   .time            = &state.time,
                   .bounce_delay_ms = 100,

                   .onRelease       = send_number
        }));

        react (Bitbang, spi,
               _ ({ .io       = &hw.io,
                    .time     = &state.time,
                    .baudrate = 9600,
                    .pins     = spi_pins,
                    .clock    = &state.clk_pin,
                    .modes    = spi_modes,
                    .buffers  = spi_buffers,

                    .onStart = spi_init,
                    .onTransmitted = spi_receive
        }));
    }

    return 0;
}
