/* Used components */
#include "routines.h"
#include <Button.h>
#include <Clock.h>
#include <IO.h>
#include <Menu.h>
#include <Serial.h>
#include <circular.h>
#include <common.h>


Clock(clk, &hw.timer, TIMESTAMP);


/* Button counter */
pin_t counter_pin = hw_pin(D, 2);
Button(counter, _({
                    .io  = &hw.io,
                    .pin = &counter_pin,

                    .type            = BUTTON_PUSH_PULLUP,
                    .clock           = &clk.state.time,
                    .bounce_delay_ms = 1000,

                    .onPress = print_counter,
                }));

/* Button push indicator */
pin_t led_pin = hw_pin(B, 1);
IO_new(led, _({
                  .io    = &hw.io,
                  .pin   = &led_pin,
                  .mode  = IO_OUTPUT,
            }));

/* Menu commands and state */
Menu(tty);
struct menu_command commands[] = {{"time", print_time},
                                  {"version", print_version},
                                  {"counter", print_counter},
                                  {"read", print_memory, command},
                                  {0}};
unsigned char       command[COMMAND_BUFFER_SIZE];

/**
 * \brief    Echo each symbol from input to output
 */
void read_symbol(Component *trigger)
{
    struct ring_buffer *input_buffer = React_State(Serial, &uart, rx_buffer);
    Serial_write(&uart, input_buffer->data[input_buffer->write - 1]);
}

/**
 * \brief    Read command from buffer
 */
void read_command(Component *trigger)
{
    unsigned char *command_symbol = command;
    unsigned char  data;

    while (Serial_read(&uart, &data) == ERROR_NONE && data) {
        *command_symbol++ = data;
    }
    *--command_symbol = 0;
}

/* UART communication */
Serial(uart, BUFFER_SIZE,
       _({
           .handler  = &hw.uart,
           .baudrate = BAUDRATE,

           .onReceiveLine = read_command, /* Read and execute command */
           .onReceive     = read_symbol,  /* Echo input */
       }));


int main(void)
{
    // Welcom log
    print_version(0);
    print_shell(0);

    loop(clk, uart, counter)
    {
        apply(Menu, tty,
              _({
                  .menu    = commands,
                  .command = command,

                  .onCommand         = print_shell,
                  .onCommandNotFound = print_command_not_found,
              }));

        apply(IO, led,
              _({
                  .level = Button_isPressed(&counter),
              }));
    }

    return 0;
}
