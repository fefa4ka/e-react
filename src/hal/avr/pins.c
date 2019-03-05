#include "pins.h"

AVRPin LED_Pin_HAL = {
    .port = {
        .port = &PORTC,
        .ddr = &DDRC,
        .pin = &PINC
    },
    .number = 4
};

AVRPin LED_Button_HAL = {
    .port = {
        .port = &PORTC,
        .ddr = &DDRC,
        .pin = &PINC
    },
    .number = 5
};