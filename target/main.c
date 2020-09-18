#include "config/device.h"


#ifdef ARCH_AVR
#include "hal/avr/api.h"
#include "hal/avr/pins.h"
#endif

#ifndef ARCH_AVR
#include "hal/virtual.h"
#include "hal/virtual_pins.c"
#define HAL Virtual_HAL
#endif

#include <log.h>
#include <circular.h>

#include <ADC.h>
#include <IO.h>
#include <UART.h>
#include <Time.h>
#include <Scheduler.h>
#include <Button.h>
#include <PWM.h>
#include <Servo.h>

//VERSION = (x << 24) + (y << 16) + (z << 8) + 0;
static const int version = 1; 

enum eCommunicationSystem {
    uart,
    spi,
    i2c
};

void LED_toggle(Component *trigger);
void ADC_readed(Component *trigger);
void Serial_transmit_completed(Component *trigger);
void String_transmitted(Component *trigger);

typedef struct
{
    rtc_datetime     time;
    event_queue      scheduler; 

    bool             led_power;
    unsigned short   brightness;
    unsigned short   angle;
    char             analog_sensor_channel;

    void             *log_stdout;

    struct sCircularBuffer 
                     buffer;
} DeviceState;

enum eAdcChannels {
    potentiomenter = 1,
    solar_panel = 2
};

#define BUFFER_SIZE 128 
unsigned char buffer[BUFFER_SIZE];

DeviceState State = {
    .led_power = true,
    .analog_sensor_channel = potentiomenter,
    .brightness = 0,
    .angle = 0,
    .buffer = { 
        .size = BUFFER_SIZE,
        .data = buffer
    },
    .scheduler = {
        .size = 0,
        .capacity = 5
    },
    .time = {0}
};

int main(void) {
    // Define React components
    React_Define(IO_block, LED_Pin);
    React_Define(IO_block, Debug_Pin);
    React_Define(Button_block, LED_Button);
    //React_Define(PWM_block, LED_Pin);
    //React_Define(IO_block, LED_Button);
    React_Define(ADC_block, Sensor_Light);
    React_Define(UART_block, Serial);
    React_Define(Time_block, Time);


    React_Define(Scheduler_block, Scheduler);
    React_Define(Servo_block, Servo);
    React_Define(Servo_block, Servo_9g);

    // Allocate memeory for events
    event events[State.scheduler.capacity];
    State.scheduler.events = events;
    State.scheduler.scheduler = &Scheduler;

    // Log direction
    State.log_stdout = &Serial;
    
    // Welcom log
    LogWithNum(common, info, "\r\ne-react ver. 0.", version);
    LogWithNum(common, info, ".", BUILD_NUM);
    Log(common, info, "\r\n"); 

    // Debug step
    bool step = true;

    // Event-loop
    while (true) { 
        // Debug Step
        step = !step;

        // Timer component, for event management and time counting
        React (Time_block) {
            .timer = &(Virtual_HAL.timer),
            .time = &State.time,
        } to(Time);

        React (Scheduler_block) {
            .timer = &(HAL.timer),
            .time = &State.time,
            .queue = &State.scheduler
        } to(Scheduler);

        React (IO_block) {
            .io = &(HAL.io),
            .pin = &LED_Pin_HAL,
            .mode = output,
            .level = State.led_power 
                ? high
                : low
        } to(LED_Pin);
        
        React (IO_block) {
            .io = &(HAL.io),
            .pin = &Debug_Pin_HAL,
            .mode = output,
            .level = step
        } to(Debug_Pin);
       
        /*
        React (Servo_block) {
            .io = &(HAL.io),
            .pin = &Servo_Pin_HAL,
            .scheduler = &Scheduler,
            .speed = 10,
            .angle = State.angle  
        } to(Servo);
        */
                                
        // UART Sender
        React (UART_block) {
            .uart = &(HAL.uart),
            .baudrate = UBRR_VALUE,
            .mode = eCommunicationModeTransceiver,
            .buffer = &State.buffer
        } to(Serial);
        
        React (ADC_block) {
            .adc = &(HAL.adc),
            .channel = &(State.analog_sensor_channel),
            .onChange = ADC_readed 
        } to(Sensor_Light);

        /*
        React (IO_block) {
            .io = &(HAL.io),
            .pin = &LED_Button_HAL,
            .mode = input,
            .onHigh = LED_toggle
        } to(LED_Button);
        */

        React (Button_block) {
            .io = &(HAL.io),
            .pin = &LED_Button_HAL,
            .type = toggle,
            .time = &State.time,
            .bounce_delay_ms = 100,
            .onToggle = LED_toggle
        } to(LED_Button);

        
        /*
        React (Servo_block) {
            .io = &(HAL.io),
            .pin = &Servo_9g_Pin_HAL,
            .scheduler = &Scheduler,
            .speed = 20,
            .angle = State.angle * 2  
        } to(Servo_9g);
      */
    }

    return 0;
}

void Timeouted(Component *trigger) {
    Log(common, info, "Timestamp: ");
    Log(common, info, utoa(State.time.time_us));
    Log(common, info, "\r\n");
}

/* Application handlers */
void LED_toggle(Component *trigger) {
    State.led_power = !State.led_power;
  
    Scheduler_enqueue(State.scheduler.scheduler, 10000, &Timeouted, trigger);

    Scheduler_enqueue(State.scheduler.scheduler, 20000, &Timeouted, trigger);

    Log(common, info, "Timestamp: ");
    Log(common, info, utoa(State.time.time_us));
    Log(common, info, " us | ");
    Log(common, info, utoa(State.time.second));
    Log(common, info, " s ");
    Log(common, info, utoa(State.time.millisecond));
    Log(common, info, " ms ");
    Log(common, info, utoa(State.time.microsecond));
    Log(common, info, " us");

    LogWithNum(common, info, " | Sensor: ", State.brightness);
    LogWithNum(common, info, " | POT: ", State.angle);
    Log(common, info, "\r\n");
}

void ADC_readed(Component *trigger) {
    ADC_blockState *state = (ADC_blockState *)trigger->state;
    
    if(State.analog_sensor_channel == potentiomenter) {
        State.analog_sensor_channel = solar_panel;
        // NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
        State.angle = state->value * 45 / 256; 

    } else if(State.analog_sensor_channel == solar_panel) {
        State.brightness = state->value;
        State.analog_sensor_channel = potentiomenter;
    }
}

/* Logging routines */
void Log(enum eLogSubSystem system, enum eLogLevel level, char *message) {
    while(*message) {
        CBWrite(&State.buffer, *(message++));
    }; 

}

void LogWithNum(enum eLogSubSystem system, enum eLogLevel level, char *message, int number) {
    Log(system, level, message);
    Log(system, level, itoa(number));
} 

void LogWithList(enum eLogSubSystem system, enum eLogLevel level, char *messages[]) {
    while(*messages) {
        char *message = *messages++;
        while(*message) {
            CBWrite(&State.buffer, *(message++));
        }
    }
}
