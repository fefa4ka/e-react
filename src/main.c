#include "config/device.h"

#include "macros/error.h"


#include "hal/avr/api.h"
#include "hal/avr/pins.h"

#include "component/ADC/ADC.h"
#include "component/IO/IO.h"


void LED_toggle(Component *trigger);
void Sensor_Light_readed(Component *trigger);


typedef struct
{
    bool led_power;
    int *brightness;
    char light_sensor_channel
} DeviceState;

DeviceState State = {
    .led_power = true,
    .brightness = NULL,
    .light_sensor_channel = 2
};


int main(void) {
    React_Define(IO_block, LED_Pin);
    React_Define(IO_block, LED_Button);
    React_Define(ADC_block, Sensor_Light);
    
    AVR_HAL.uart.init(UART_BAUDRATE);
    stdout = AVR_HAL.uart.stream;
    stdin = AVR_HAL.uart.stream;

    while (true) {
        errno += 1;

        React (IO_block) {
            .io = &(AVR_HAL.io),
            .pin = &LED_Pin_HAL,
            .mode = output,
            .level = State.led_power 
                ? high
                : low
        } to(LED_Pin); 

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


void LED_toggle(Component *trigger) {
    State.led_power = !State.led_power;
    printf("b %d\n", errno);
}

void Sensor_Light_readed(Component *trigger) {
    ADC_blockState *state = (ADC_blockState *)trigger->state;

    State.brightness = &state->value;  // 255;

    printf("l %d\n", *State.brightness);
}
