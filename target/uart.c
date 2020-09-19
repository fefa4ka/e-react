#include "config/device.h"
#include <log.h>

static const int version = 1; 

#include "hal/avr/api.h"
#include <Time.h>
#include <Button.h>
#include <UART.h>

#define HW AVR_HAL
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
    struct ring_buffer_s 
                     buffer;
} device_state_t;

#define BUFFER_SIZE 128 
unsigned char buffer[BUFFER_SIZE];

device_state_t state  = {
    .time = {0},
    .buffer = { 
        .size = BUFFER_SIZE,
        .data = buffer
    },
};

void print_version();
void print_time();
void button_toggle(Component *trigger);
void log(char *message);

/* Application handlers */
void button_toggle(Component *trigger)
{
    log("Button toggle\r\n");
}

void button_press(Component *trigger)
{
    log("Button press\r\n");
}

void button_release(Component *trigger)
{
    log("Button release\r\n");
}

void second_tick(Component *trigger)
{
    print_time();
}

void minute_tick(Component *trigger)
{
    log("Next minute\r\n");
}

int main(void)
{
    // Define React components
    react_define(Time, datetime);
    react_define(UART, serial);
    react_define(Button, button);

    // Welcom log
    print_version();

    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(HW.timer),
            .time = &state.time,
            .onSecond = second_tick,
            .onMinute = minute_tick
        } to (datetime);

        react (UART) {
            .uart = &(HW.uart),
            .baudrate = UBRR_VALUE,
            .mode = eCommunicationModeTransceiver,
            .buffer = &state.buffer
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
    }

    return 0;
}


void log(char *message) {
    while(*message) {
        rb_write(&state.buffer, *(message++));
    }; 
}

void log_num(char *message, int number) {
    log(message);
    log(itoa(number));
} 

void print_version() {
    log_num("\r\ne-react ver. 0.", version);
    log_num(".", BUILD_NUM);
    log("\r\n"); 
}

void print_time() {
    log("Timestamp: ");
    log(utoa(state.time.time_us));
    log(" us | ");
    log(utoa(state.time.second));
    log(" s ");
    log(utoa(state.time.millisecond));
    log(" ms ");
    log(utoa(state.time.microsecond));
    log(" us");
    log("\r\n");
}
