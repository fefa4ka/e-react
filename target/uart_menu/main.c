/* Used components */
#include "config.h"

unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device state  = {
    .time           = {0},
    .command        = {0},
    .output_buffer  = { output_buffer, BUFFER_SIZE },
    .input_buffer   = { input_buffer, BUFFER_SIZE },
    .btn_pin = hw_pin(D, 0)
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
    print_shell(NULL);

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

            .onReceiveLine = read_command,
            .onReceive = read_symbol
        }));


        react (Button, button, _({
            .io = &hw.io,
            .pin = &state.btn_pin,

            .type = BTN_PUSH_PULLUP, 
            .time = &state.time,
            .bounce_delay_ms = 1000,

            .onPress = print_version,
            }));

        react (Menu, tty, _({
            .menu = commands,
            .command = (unsigned char*)&state.command,

            .onCommand = print_shell,
        }));
        

        react (IO, led,
               _ ({ .io    = &hw.io,
                    .pin   = led_pin,
                    .mode  = IO_OUTPUT,
                    .level = Button_isPressed(&button) }));
    }

    return 0;
}


