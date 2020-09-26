#include "config/device.h"

#include <AtDC.h>
#include <IO.h>
#include <PWM.h>
#include <UART.h>

#include "hal/avr/api.h"

AVRPin led_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 1
};


#define HW AVR_HAL


enum sensor_channel_e {
    potentiomenter = 0,
    solar_panel = 1
};

#define BUFFER_SIZE 128 
unsigned char buffer[BUFFER_SIZE];
typedef struct
{
    rtc_datetime_t   time;
    enum sensor_channel_e 
                     sensor;
    unsigned short   brightness;
    unsigned short   angle;
    struct ring_buffer_s 
                     buffer;
} device_state_t ;

device_state_t state = {
    .time = {0},
    .sensor = potentiomenter,
    .brightness = 0,
    .angle = 0,
    .buffer = { 
        .size = BUFFER_SIZE,
        .data = buffer
    },
};

/* Application handlers */
void log_string(char *message) {
    rb_write_string(&state.buffer, message);
}

void log_num(char *message, int number) {
    rb_write_string(&state.buffer, message);
    rb_write_string(&state.buffer, itoa(number));
} 

void sensor_readed(Component *trigger) {
    AtDC_blockState *adc_state = (AtDC_blockState *)trigger->state;
    
    if(state.sensor == potentiomenter) {
        state.sensor = solar_panel;
        // NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
        state.angle = adc_state->value * 45 / 256; 

    } else if(state.sensor == solar_panel) {
        state.brightness = adc_state->value;
        state.sensor = potentiomenter;
    }
}

void log_sensors(Component *trigger)
{
    log_num("\r\nLight: ", state.brightness);
    log_num("\r\nPOT: ", state.angle);
}


int main(void) {
    // Define React components
    react_define(Time, datetime);
    react_define(PWM, led);
    react_define(AtDC, sensor);
    react_define(UART, serial);

    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(HW.timer),
            .time = &state.time,
            .onSecond = log_sensors
        } to (datetime);

        react (UART) {
            .uart = &(HW.uart),
            .baudrate = UBRR_VALUE,
            .mode = eCommunicationModeTransceiver,
            .buffer = &state.buffer
        } to (serial);

        react (AtDC) {
            .adc = &(HW.adc),
            .channel = &(state.sensor),
            .onChange = sensor_readed 
        } to (sensor);

        react (PWM) {
            .io = &(HW.io),
            .pin = &led_pin_hw,
            .frequency = 20,
            .duty_cycle = state.brightness,
            .time = &state.time
        } to (led);
    }

    return 0;
}


