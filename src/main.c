#include "config/device.h"

#include "macros/error.h"


#include "hal/avr/api.h"
#include "hal/avr/pins.h"

#include "utils/log.h"
#include "utils/circular.h"

#include "component/ADC/ADC.h"
#include "component/IO/IO.h"
#include "component/UART/UART.h"
//#include "component/Button/Button.h"

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
    unsigned int     time;

    bool             led_power;
    unsigned short   brightness;
    char             light_sensor_channel;

    struct sCircularBuffer 
                     buffer;
} DeviceState;

#define BUFFER_SIZE 255
unsigned char buffer[BUFFER_SIZE];
unsigned char buffer_owner[BUFFER_SIZE];

DeviceState State = {
    .led_power = true,
    .brightness = 0,
    .light_sensor_channel = 2,
    .time = 0,
    .buffer = { 
        .size = BUFFER_SIZE,
        .owner = buffer_owner,
        .data = buffer,
        .read = 0,
        .write = 0 
    }
};

int main(void) {
    TCCR1B |= (1 << CS11) | (1 << CS10);

    React_Define(IO_block, LED_Pin);
    //React_Define(Button_block, LED_Button);
    React_Define(IO_block, LED_Button);
    React_Define(ADC_block, Sensor_Light);
    React_Define(UART_block, Serial);

    while (true) { //(State.time = AVR_HAL.time())) {
        State.time = AVR_HAL.time(); 

        React (IO_block) {
            .io = &(AVR_HAL.io),
            .pin = &LED_Pin_HAL,
            .mode = output,
            .level = State.led_power 
                ? high
                : low
        } to(LED_Pin);
        
        
        // UART Sender
        if(CBReadOwner(&State.buffer, uart) != eErrorBufferBusy)
            React (UART_block) {
                .uart = &(AVR_HAL.uart),
                .baudrate = UBRR_VALUE,
                .mode = transmiter,
                .buffer = &State.buffer
            } to(Serial);
     


        
        React (IO_block) {
            .io = &(AVR_HAL.io),
            .pin = &LED_Button_HAL,
            .mode = input,
            .onHigh = LED_toggle
        } to(LED_Button);
        

        React (ADC_block) {
            .adc = &(AVR_HAL.adc),
            .channel = &(State.light_sensor_channel),
            .onChange = Sensor_Light_readed
        } to(Sensor_Light);
    }

    return 0;
}

/* Application handlers */
void LED_toggle(Component *trigger) {
    State.led_power = !State.led_power;
  
    Log(common, info, "\r\n- LED Toggle\r\n");
    Log(common, info, "Time: ");
    Log(common, info, utoa(State.time));

    LogWithNum(common, info, " | Sensor: ", State.brightness);
    Log(common, info, "\r\n");
}

void Sensor_Light_readed(Component *trigger) {
    ADC_blockState *state = (ADC_blockState *)trigger->state;

    State.brightness = state->value;  // 255;
}


/* Logging routines */
void Log(enum eLogSubSystem system, enum eLogLevel level, char *message) {
    while(*message) {
        CBWriteOwner(&State.buffer, *(message++), uart);
    }; 

}

void LogWithNum(enum eLogSubSystem system, enum eLogLevel level, char *message, int number) {
    Log(system, level, message);
    Log(system, level, itoa(number));
} 

