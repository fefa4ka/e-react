#include <IO.h>
#include <Button.h>
#include <Calendar.h>
#include <SIPO.h>

struct device
{
    struct rtc_datetime time;

    pin_t           clk_pin;
    pin_t           data_pin;
    pin_t           reset_pin;
    pin_t           btn_pin;
    struct SIPO_pin led_pin;
};
struct device state = { .time      = { 0 },
                        .clk_pin   = hw_pin (D, 2),
                        .data_pin  = hw_pin (D, 3),
                        .reset_pin = hw_pin (D, 4),
                        .btn_pin   = hw_pin (D, 4) };


int
main (void)
{
    // Define React components
    Time (datetime);
    IO (led);
    Button (pusher);
    SIPO (gpio_array);

    state.led_pin.bus    = &gpio_array;
    state.led_pin.number = 1;

    // Event-loop
    while (true) {
        // Timer component, for event management and time counting
        react (Time, datetime,
               _ ({
                   .timer = &hw.timer,
                   .time  = &state.time,
               }));

        react (IO, led,
               _ ({ .io    = &SIPO_handler,
                    .pin   = &state.led_pin,
                    .mode  = IO_OUTPUT,
                    .level = Button_isPressed(&pusher) }));

        react (Button, pusher,
               _ ({
                   .io              = &hw.io,
                   .pin             = &state.btn_pin,
                   .type            = BTN_PUSH_PULLUP,
                   .time            = &state.time,
                   .bounce_delay_ms = 100,
               }));

        react (SIPO, gpio_array,
               _ ({
                   .io   = &hw.io,
                   .time = &state.time,

                   .baudrate = 9600,

                   .data_pin  = &state.data_pin,
                   .reset_pin = &state.reset_pin,
                   .clk_pin   = &state.clk_pin,
               }));
    }

    return 0;
}
