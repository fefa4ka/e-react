#include "config.h"

unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device state = {
    .time          = { 0 },
    .clk_pin       = hw_pin (D, 4),
    .mosi_pin      = hw_pin (D, 5),
    .miso_pin      = hw_pin (B, 7),
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
log_string (char *message)
{
    while (*message) {
        rb_write (&state.output_buffer, *(message++));
    };
}

void
log_num (char *message, int number)
{
    log_string (message);
    log_string (itoa (number));
}

void
send_version (Component *trigger)
{
    log_num ("\r\ne-react ver. 0.", VERSION);
    log_num (".", BUILD_NUM);
    log_string ("\r\n");
}

static const long hextable[]
    = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,
        6,  7,  8,  9,  -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1 };

/**
 * @brief convert a hexidecimal string to a signed long
 * will not produce or process negative numbers except
 * to signal error.
 *
 * @param hex without decoration, case insensitive.
 *
 * @return -1 on error, or result (max (sizeof(long)*8)-1 bits)
 */
long
hexdec (unsigned const char *hex)
{
    long ret = 0;
    while (*hex && ret >= 0) {
        ret = (ret << 4) | hextable[*hex++];
    }
    return ret;
}
int
main (void)
{
    // Define React components
    Time (datetime);
    Button (button);
    Bitbang (spi);

    // Event-loop
    while (true) {
        // Timer component, for event management and time counting
        react (Time, datetime,
               _ ({
                   .timer = &hw.timer,
                   .time  = &state.time,
               }));

        react (Button, button,
               _ ({ .io              = &hw.io,
                    .pin             = signal_pin,
                    .type            = BTN_PUSH_PULLUP,
                    .time            = &state.time,
                    .bounce_delay_ms = 100,
                    .onRelease       = send_version }));


        react (Bitbang, spi,
               _ ({ .io       = &hw.io,
                    .time     = &state.time,
                    .baudrate = 9600,
                    .pins     = spi_pins,
                    .clock    = &state.clk_pin,
                    .modes    = spi_modes,
                    .buffers  = spi_buffers,

                    .onTransmit = send_number }));
    }

    return 0;
}
