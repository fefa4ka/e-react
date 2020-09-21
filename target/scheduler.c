#include "config/device.h"

#include <AtDC.h>
#include <IO.h>
#include <UART.h>
#include <Button.h>
#include <Scheduler.h>
#include <Servo.h>

#include "hal/avr/api.h"

AVRPin led_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 1
};

AVRPin button_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 2
};

AVRPin motor_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 3
};
AVRPin servo_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 4
};
AVRPin nervo_pin_hw = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 5
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
    event_queue      scheduler; 

    enum sensor_channel_e 
                     sensor;
    unsigned short   delay;
    unsigned short   angle;
    struct ring_buffer_s 
                     buffer;
    bool             motor_enabled;
} device_state_t ;

device_state_t state = {
    .time = {0},
    .sensor = potentiomenter,
    .delay = 0,
    .angle = 0,
    .buffer = { 
        .size = BUFFER_SIZE,
        .data = buffer
    },
    .motor_enabled = true,
    .scheduler = {
        .size = 0,
        .capacity = 15 
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
    
    state.delay = adc_state->value * 20; 
    state.angle = adc_state->value * 45 / 256; 
}

void delayed_message(Component *trigger)
{
    log_string("\r\nDelayed message\r\n");
    log_num("Angle: ", state.angle);

}

void button_release(Component *trigger)
{
    log_num("\r\nButton release, delay: ", state.delay);

}

void switch_motor(Component *trigger) {
    state.motor_enabled = !state.motor_enabled;
    log_num("\r\nMotor: ", state.motor_enabled);
    bool scheduled = Scheduler_enqueue(
        state.scheduler.scheduler, 
        state.delay, 
        delayed_message, trigger
    );
}

void print_version() {
    log_num("\r\nscheduler ver. ", BUILD_NUM);
    log_string("\r\n"); 
}

int main(void) {
    // Define React components
    react_define(Time, datetime);
    react_define(AtDC, sensor);
    react_define(UART, serial);
    react_define(Button, button);
    react_define(Scheduler, scheduler);
    react_define(Servo, motor);
    react_define(Servo, servo);
    react_define(Servo, nervo);

    // Allocate memeory for events
    event events[state.scheduler.capacity];
    state.scheduler.events = events;
    state.scheduler.scheduler = &scheduler;

    print_version();
    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(HW.timer),
            .time = &state.time
        } to (datetime);

        react (Scheduler) {
            .timer = &(HW.timer),
            .time = &state.time,
            .queue = &state.scheduler,
        } to (scheduler);

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

        react (Button) {
            .io = &(HW.io),
            .pin = &button_pin_hw,
            .type = toggle, /* or toggle */
            .time = &state.time,
            .bounce_delay_ms = 50,
            .onRelease = button_release,
            .onToggle = switch_motor
        } to (button);

        react (Servo) {
            .io = &(HW.io),
            .pin = &motor_pin_hw,
            .scheduler = &scheduler,
            .speed = 10,
            .enabled = state.motor_enabled,
            .angle = state.angle  
        } to (motor);

        /*
        react (Servo) {
            .io = &(HW.io),
            .pin = &servo_pin_hw,
            .scheduler = &scheduler,
            .speed = 30,
            .enabled = state.motor_enabled,
            .angle = 180 - state.angle  
        } to (servo);

        react (Servo) {
            .io = &(HW.io),
            .pin = &nervo_pin_hw,
            .scheduler = &scheduler,
            .speed = 15,
            .enabled = state.motor_enabled,
            .angle = 180 - state.angle  
        } to (nervo);
        */
    }

    return 0;
}


