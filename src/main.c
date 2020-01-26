#include "config/device.h"

#include "macros/error.h"


#include "hal/avr/api.h"
#include "hal/avr/pins.h"

#include "utils/log.h"
#include "utils/circular.h"

#include "component/ADC/ADC.h"
#include "component/IO/IO.h"
#include "component/UART/UART.h"
#include "component/Time/Time.h"
#include "component/Button/Button.h"

enum eCommunicationSystem {
    uart,
    spi,
    i2c
};

void LED_toggle(Component *trigger);
void Sensor_Light_readed(Component *trigger);
void Serial_transmit_completed(Component *trigger);
void String_transmitted(Component *trigger);

typedef struct
{
    rtc_datetime_24h_t time;

    bool             led_power;
    unsigned short   brightness;
    unsigned short   strength;
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
void *buffer_owner[BUFFER_SIZE];

DeviceState State = {
    .led_power = true,
    .analog_sensor_channel = potentiomenter,
    .brightness = 0,
    .strength = 0,
    .buffer = { 
        .size = BUFFER_SIZE,
        .owner = buffer_owner,
        .data = buffer
    },
    .time = {0}
};

int main(void) {

    React_Define(IO_block, LED_Pin);
    React_Define(Button_block, LED_Button);
    //React_Define(IO_block, LED_Button);
    React_Define(ADC_block, Sensor_Light);
    React_Define(UART_block, Serial);
    React_Define(Time_block, Time);

    State.log_stdout = &Serial;
    

    Log(common, info, "\r\nApplication loaded\r\n");

    while (true) { 
        React (Time_block) {
            .timer = &(AVR_HAL.timer),
            .time = &State.time
        } to(Time);

        React (IO_block) {
            .io = &(AVR_HAL.io),
            .pin = &LED_Pin_HAL,
            .mode = output,
            .level = State.led_power 
                ? high
                : low
        } to(LED_Pin);
        
        
        // UART Sender
        if(CBReadOwner(&State.buffer, &Serial) != eErrorBufferBusy)
            React (UART_block) {
                .uart = &(AVR_HAL.uart),
                .baudrate = UBRR_VALUE,
                .mode = transmiter,
                .buffer = &State.buffer
            } to(Serial);
    
        React (ADC_block) {
            .adc = &(AVR_HAL.adc),
            .channel = &(State.analog_sensor_channel),
            .onChange = Sensor_Light_readed
        } to(Sensor_Light);

        /*
        React (IO_block) {
            .io = &(AVR_HAL.io),
            .pin = &LED_Button_HAL,
            .mode = input,
            .onHigh = LED_toggle
        } to(LED_Button);
        */

        React (Button_block) {
            .io = &(AVR_HAL.io),
            .pin = &LED_Button_HAL,
            .type = push,
            .time = &State.time,
            .bounce_delay_ms = 50,
            .onToggle = LED_toggle
        } to(LED_Button);
      
    }

    return 0;
}

/* Application handlers */
void LED_toggle(Component *trigger) {
    State.led_power = !State.led_power;
  
    char *time[] = { "\r\n- LED Toggle\r\nTime: ", utoa(State.time.second), " s ", utoa(State.time.millisecond), " ms ", utoa(State.time.microsecond), " m", NULL};
    
    LogWithList(common, info, time);
    LogWithNum(common, info, " | Sensor: ", State.brightness);
    LogWithNum(common, info, " | POT: ", State.strength);
    Log(common, info, "\r\n");
}

void Sensor_Light_readed(Component *trigger) {
    ADC_blockState *state = (ADC_blockState *)trigger->state;
    
    if(State.analog_sensor_channel == potentiomenter) {
        State.analog_sensor_channel = solar_panel;
        State.strength = state->value;
    } else if(State.analog_sensor_channel == solar_panel) {
        State.brightness = state->value;
        State.analog_sensor_channel = potentiomenter;
    }
}

void POT_strength_readed(Component *trigger) {
    ADC_blockState *state = (ADC_blockState *)trigger->state;

    State.strength = state->value;  // 255;
}


/* Logging routines */
void Log(enum eLogSubSystem system, enum eLogLevel level, char *message) {
    while(*message) {
        CBWriteOwner(&State.buffer, *(message++), State.log_stdout);
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
            CBWriteOwner(&State.buffer, *(message++), State.log_stdout);
        }
    }
}
