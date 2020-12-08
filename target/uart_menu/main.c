/* Used components */
#include "config.h"

unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device state  = {
    .time           = {0},
    .command        = {0},
    .output_buffer  = { output_buffer, BUFFER_SIZE },
    .input_buffer   = { input_buffer, BUFFER_SIZE }
};


struct menu_command commands[] = {
    { "time", print_time },
    { "version", print_version }
};


// Define React components
Time(datetime);
Serial(uart);
Button(button);
Menu(tty);

IO (led);

int main(void) {
    // Welcom log
    print_version(NULL);

    // Event-loop
    while(true) { 
        // Timer component, for event management and time counting
        react (Time, datetime, _({
            .timer = &hw.timer,

            .time = &state.time,

            .onMinute = print_time
        }));


        react (Serial, uart, _({
            .handler = &hw.uart,

            .baudrate = BAUDRATE,
            .tx_buffer = &state.output_buffer,
            .rx_buffer = &state.input_buffer,

            .onReceiveLine = read_command 
        }));


        react (Button, button, _({
            .io = &hw.io,
            .pin = button_pin,

            .type = BTN_PUSH_PULLUP, 
            .time = &state.time,
            .bounce_delay_ms = 1000,

            .onPress = print_version,
            }));

        react (Menu, tty, _({
            .menu = commands,
            .command = (unsigned char*)&state.command,

            .onCommand = print_command,
        }));
        

        react (IO, led,
               _ ({ .io    = &hw.io,
                    .pin   = led_pin,
                    .mode  = IO_OUTPUT,
                    .level = Button_State (button, pressed) }));
    }

    return 0;
}


