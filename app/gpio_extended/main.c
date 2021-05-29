#include <Button.h>
#include <Calendar.h>
#include <IO.h>
#include <SIPO.h>

struct device
{
    struct rtc_datetime time;

    pin_t           clk_pin;
    pin_t           data_pin;
    pin_t           reset_pin;
    pin_t           btn_pin;
    struct SIPO_pin led_pin;

    struct ring_buffer     input_buffer;
    struct ring_buffer     output_buffer;

    struct SIPO_pin spi_clk_pin;
    struct SIPO_pin miso_pin;
    struct SIPO_pin mosi_pin;

    unsigned char          index; 
};

SIPO (gpio_array);

#define   BUFFER_SIZE          8
unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device state = {
    .time          = { 0 },
    .led_pin       = { &gpio_array, 1 },
    .clk_pin       = hw_pin (D, 2),
    .data_pin      = hw_pin (D, 3),
    .reset_pin     = hw_pin (D, 4),
    .btn_pin       = hw_pin (D, 0),
    .spi_clk_pin   = { &gpio_array, 2 },
    .mosi_pin      = { &gpio_array, 0 },
    .miso_pin      = { &gpio_array, 3 },
    .input_buffer  = { input_buffer, BUFFER_SIZE },
    .output_buffer = { output_buffer, BUFFER_SIZE },
    .index         = 0,
};
pin_t *             spi_pins[] = { &state.mosi_pin, &state.miso_pin, NULL };
struct ring_buffer *spi_buffers[]
    = { &state.output_buffer, &state.input_buffer };
enum pin_mode spi_modes[] = { PIN_MODE_OUTPUT, PIN_MODE_INPUT };

/* Application handlers */
void
send_number (Component *trigger)
{
    state.index++;

    rb_write (&state.output_buffer, state.index);
}

int
main (void)
{
    // Define React components
    Time (datetime);
    IO (led);
    Bitbang (spi);
    Button (pusher);

    // Event-loop
    while (true) {
        // Timer component, for event management and time counting
        react (Time, datetime,
               _ ({
                   .timer = &hw.timer,
                   .time  = &state.time,
               }));

       /* 
        react (IO, led,
               _ ({ .io    = &SIPO_handler,
                    .pin   = &state.led_pin,
                    .mode  = IO_OUTPUT,
                    .level = Button_isPressed (&pusher) }));
                    */

        react (Button, pusher,
               _ ({ .io              = &hw.io,
                    .pin             = &state.btn_pin,
                    .type            = BTN_PUSH_PULLUP,
                    .time            = &state.time,
                    .bounce_delay_ms = 100,

                    .onRelease = send_number }));

        react (Bitbang, spi,
               _ ({
                   .io       = &SIPO_handler,
                   .time     = &state.time,
                   .baudrate = 200,
                   .pins     = spi_pins,
                   .clock    = &state.spi_clk_pin,
                   .modes    = spi_modes,
                   .buffers  = spi_buffers,
               }));

        react (SIPO, gpio_array,
               _ ({
                   .io   = &hw.io,
                   .time = &state.time,

                   .data_pin  = &state.data_pin,
                   .reset_pin = &state.reset_pin,
                   .clk_pin   = &state.clk_pin,
               }));
    }

    return 0;
}
