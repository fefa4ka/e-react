#include "config/device.h"

#include <ADC.h>
#include <IO.h>
#include <PWM.h>

#ifdef ARCH_AVR
#include "hal/avr/api.h"

AVRPin LED_Pin_HW = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 2
};

AVRPin LED_Button_HW = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 1
};

#define HW AVR_HAL
#endif

#ifdef ARCH_x86
#include "hal/virtual.h"
VirtualPin LED_Pin_HW = {
    .port = 'B',
    .number = 2
};

VirtualPin LED_Button_HW = {
    .port = 'B',
    .number = 1
};
#define HW Virtual_HAL
#endif

enum eSensorChannels {
    potentiomenter = 1,
    solar_panel = 2
};

typedef struct
{
    rtc_datetime     time;
    enum eSensorChannels sensor;
    unsigned short   brightness;
    unsigned short   angle;
} DeviceState;

DeviceState State = {
    .time = {0},
    .sensor = potentiomenter,
    .brightness = 0,
    .angle = 0
};

/* Application handlers */
void Sensor_readed(Component *trigger) {
    ADC_blockState *state = (ADC_blockState *)trigger->state;
    
    if(State.sensor == potentiomenter) {
        State.sensor = solar_panel;
        // NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
        State.angle = state->value * 45 / 256; 

    } else if(State.sensor == solar_panel) {
        State.brightness = state->value;
        State.sensor = potentiomenter;
    }
}



int main(void) {
    // Define React components
    React_Define(Time_block, Time);
    React_Define(PWM_block, LED_Pin);
    React_Define(ADC_block, Sensor);


    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        React (Time_block) {
            .timer = &(HW.timer),
            .time = &State.time,
        } to(Time);

        React (ADC_block) {
            .adc = &(HW.adc),
            .channel = &(State.sensor),
            .onChange = Sensor_readed 
        } to(Sensor);

        React (PWM_block) {
            .io = &(HW.io),
            .pin = &LED_Pin_HW,
            .frequency = 20,
            .duty_cycle = State.brightness,
            .time = &State.time
        } to(LED_Pin);
    }

    return 0;
}


