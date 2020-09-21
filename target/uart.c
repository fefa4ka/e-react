#include "config/device.h"

#include <Button.h>
#include <UART.h>
#include <Menu.h>

#include "hal/avr/api.h"
#define HW AVR_HAL
static const int version = 1; 
AVRPin button_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 2
};

typedef struct
{
    rtc_datetime_t    time;
    unsigned char     command[32];
    struct ring_buffer_s 
                      output_buffer;
    struct ring_buffer_s 
                      input_buffer;
} device_state_t;

#define BUFFER_SIZE 128 
unsigned char output_buffer[BUFFER_SIZE];

#define BUFFER_SIZE 128 
unsigned char input_buffer[BUFFER_SIZE];

device_state_t state  = {
    .time = {0},
    .command = {0},
    .output_buffer = { 
        .size = BUFFER_SIZE,
        .data = output_buffer
    },
    .input_buffer = { 
        .size = BUFFER_SIZE,
        .data = input_buffer
    },
};

void print_version(void *args);
void print_time(void *args);
void button_toggle(Component *trigger);
void log_string(char *message);


/* Application handlers */
void button_toggle(Component *trigger)
{
    log_string("Button toggle\r\n");
}

void button_press(Component *trigger)
{
    log_string("Button press\r\n");
}

void button_release(Component *trigger)
{
    log_string("Button release\r\n");
}

void second_tick(Component *trigger)
{
    print_time(NULL);
}

void minute_tick(Component *trigger)
{
    log_string("Next minute\r\n");
}

void menu_lookup(Component *trigger)
{
}

void menu_select(Component *trigger)
{
}

void menu_run(Component *trigger)
{
}

void print_command(Component *trigger) {
    log_string("\r\n$ > ");
    log_string(state.command);
    log_string("\r\n");
}

void read_command(Component *trigger) 
{
    unsigned char *command = state.command;
    unsigned char data;

    while(rb_read(&state.input_buffer, &data) == eErrorNone && data) {
        *command++ = data;
    }
    *--command = 0;
}

int main(void)
{
    // Define React components
    react_define(Time, datetime);
    react_define(UART, serial);
    react_define(Button, button);
    react_define(Menu, tty);

    menu_command_t commands[] = {
        { 
            .command = "time", 
            .callback = print_time 
        },
        { 
            .command = "version", 
            .callback = print_version 
        },
        { 0 }
    };

    // Welcom log
    print_version(NULL);

    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(HW.timer),
            .time = &state.time,
            .onMinute = minute_tick
        } to (datetime);

        react (UART) {
            .uart = &(HW.uart),
            .baudrate = UBRR_VALUE,
            .tx_buffer = &state.output_buffer,
            .rx_buffer = &state.input_buffer,
            .onReceiveLine = read_command 
        } to (serial);

        react (Button) {
            .io = &(HW.io),
            .pin = &button_pin_hw,
            .type = push, /* or toggle */
            .time = &state.time,
            .bounce_delay_ms = 1000,
            .onToggle = button_toggle,
            .onPress = button_press,
            .onRelease = button_release
        } to (button);

        react (Menu) {
            .menu = commands,
            .command = &state.command,
            .onCommand = print_command,
            .onLookup = menu_lookup,
            .onSelect = menu_select,
            .onWillRun = menu_run
        } to (tty);
        
    }

    return 0;
}


void log_string(char *message) {
    while(*message) {
        rb_write(&state.output_buffer, *(message++));
    }; 
}

void log_num(char *message, int number) {
    log_string(message);
    log_string(itoa(number));
} 

void print_version(void *args) {
    log_num("\r\ne-react ver. 0.", version);
    log_num(".", BUILD_NUM);
    log_string("\r\n"); 
}

void print_time(void *args) {
    log_string("Timestamp: ");
    log_string(utoa(state.time.time_us));
    log_string(" us | ");
    log_string(utoa(state.time.second));
    log_string(" s ");
    log_string(utoa(state.time.millisecond));
    log_string(" ms ");
    log_string(utoa(state.time.microsecond));
    log_string(" us");
    log_string("\r\n");
}
