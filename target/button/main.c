#include <Button.h>
#include <Calendar.h>
#include <UART.h>
#include <circular.h>


#define BAUDRATE    9600
#define BUFFER_SIZE 128
unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device
{
    struct rtc_datetime time;
    struct ring_buffer  output_buffer;
    struct ring_buffer  input_buffer;
    pin_t               btn_push_pin;
    pin_t               btn_switch_pin;
    pin_t               led_pin;
    pin_t               led_red_pin;
};

struct device state = { .time           = { 0 },
                        .output_buffer  = { output_buffer, BUFFER_SIZE },
                        .input_buffer   = { input_buffer, BUFFER_SIZE },
                        .led_pin        = hw_pin (B, 1),
                        .led_red_pin    = hw_pin (B, 2),
                        .btn_push_pin   = hw_pin (D, 0),
                        .btn_switch_pin = hw_pin (D, 1) };

void
log_string (char *message)
{
    while (*message) {
        rb_write (&state.output_buffer, *(message++));
    };
}

Time (datetime);
IO (led);
IO (led_red);
Button (switcher);
Button (pusher);

int
main (void)
{
    // Event-loop
    while (true) {

        // Timer component, for event management and time counting
        react (Time, datetime, _ ({ .timer = &hw.timer, .time = &state.time }));

        react (IO, led,
               _ ({ .io    = &hw.io,
                    .pin   = &state.led_pin,
                    .mode  = IO_OUTPUT,
                    .level = Button_isPressed (&switcher) }));

        react (IO, led_red,
               _ ({ .io    = &hw.io,
                    .pin   = &state.led_red_pin,
                    .mode  = IO_OUTPUT,
                    .level = Button_isPressed (&pusher) }));

        react (Button, switcher,
               _ ({
                   .io              = &hw.io,
                   .pin             = &state.btn_switch_pin,
                   .type            = BTN_TOGGLE_PULLUP,
                   .time            = &state.time,
                   .bounce_delay_ms = 1000,
               }));

        react (Button, pusher,
               _ ({
                   .io              = &hw.io,
                   .pin             = &state.btn_push_pin,
                   .type            = BTN_PUSH_PULLUP,
                   .time            = &state.time,
                   .bounce_delay_ms = 100,
               }));
    }

    return 0;
}
