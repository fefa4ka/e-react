#include "pins.h"

AVRPin LED_Pin_HAL = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 2
};

AVRPin LED_Button_HAL = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 1
};

AVRPin Servo_Pin_HAL = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 0
};

AVRPin Servo_9g_Pin_HAL = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 5
};

AVRPin Debug_Pin_HAL = {
    .port = {
        .port = &PORTB,
        .ddr = &DDRB,
        .pin = &PINB
    },
    .number = 3
};
