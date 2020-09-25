/* Used components */
#include <Button.h>
#include <UART.h>
#include <Menu.h>

#include "config.h"
#include "routines.h"

unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device state  = {
    .time = {0},
    .command = {0},
    .output_buffer = { output_buffer, BUFFER_SIZE },
    .input_buffer = { input_buffer, BUFFER_SIZE }
};

hw_pin(button_pin, B, 1);

struct menu_command commands[] = {
    { "time", print_time },
    { "version", print_version }
};

int main(void) {
    // Define React components
    component(Time, datetime);
    component(UART, serial);
    component(Button, button);
    component(Menu, tty);

    
    // Welcom log

    // Event-loop
    for (;;) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(hw.timer),

            .time = &state.time,

            .onMinute = print_time
        } to (datetime);


        react (UART) {
            .uart = &(hw.uart),

            .baudrate = BAUDRATE,
            .tx_buffer = &state.output_buffer,
            .rx_buffer = &state.input_buffer,

            .onReceiveLine = read_command 
        } to (serial);


        react (Button) {
            .io = &(hw.io),
            .pin = &button_pin,

            .type = push, 
            .time = &state.time,
            .bounce_delay_ms = 1000,

            .onPress = print_version,
        } to (button);

        react (Menu) {
            .menu = commands,
            .command = &state.command,

            .onCommand = print_command,
        } to (tty);
    }

    return 0;
}


