#include <Button.h>
#include <Calendar.h>
#include <UART.h>
#include <circular.h>

struct rtc_datetime time = {0};

#define   BAUDRATE     9600
#define   BUFFER_SIZE          128 
unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device
{
    struct ring_buffer     output_buffer;
    struct ring_buffer     input_buffer;

};
struct device state  = {
    .output_buffer  = { output_buffer, BUFFER_SIZE },
    .input_buffer   = { input_buffer, BUFFER_SIZE }
};

void log_string(char *message) 
{
    while(*message) {
        rb_write(&state.output_buffer, *(message++));
    }; 
}



    // Define React components
    Time (datetime);
    IO (led);
    IO (led_yet);
    Button (switcher);
    Button (pusher);

    UART(serial);
int
main (void)
{

    log_string("Welcome\r\n");

    // Event-loop
    while (true) {

        // Timer component, for event management and time counting
        react (Time, datetime,
               _ ({ .timer = &(hw.timer), .time = &time }));

        react (IO, led_yet,
               _ ({ .io    = &hw.io,
                    .pin   = hw_pin (D, 4),
                    .mode  = IO_OUTPUT,
                    .level = Button_State (pusher, pressed) }));

        react (IO, led,
               _ ({ .io    = &hw.io,
                    .pin   = hw_pin (D, 5),
                    .mode  = IO_OUTPUT,
                    .level = Button_State (switcher, pressed) }));

        react (Button, switcher,
               _ ({
                   .io              = &hw.io,
                   .pin             = hw_pin (D, 6),
                   .type            = BTN_TOGGLE_PULLUP,
                   .time            = &time,
                   .bounce_delay_ms = 1000,
               }));

        react (UART, serial, _({
            .uart = &hw.uart,

            .baudrate = BAUDRATE,
            .tx_buffer = &state.output_buffer,
            .rx_buffer = &state.input_buffer,

        }));

        react (Button, pusher,
               _ ({
                   .io              = &hw.io,
                   .pin             = hw_pin (D, 7),
                   .type            = BTN_PUSH_PULLUP,
                   .time            = &time,
                   .bounce_delay_ms = 100,
               }));
    }

    return 0;
}
