#pragma once

#include <IO.h>
#include <Button.h>
#include <Serial.h>
#include <Menu.h>
#include <Calendar.h>
#include <circular.h>
#include "routines.h"

#define   VERSION      1
#define   BAUDRATE     9600

#define   COMMAND_BUFFER_SIZE  32
#define   BUFFER_SIZE          128 

#define button_pin hw_pin (D, 7)
#define led_pin    hw_pin (D, 5)

struct device
{
    struct rtc_datetime    time;

    struct ring_buffer     output_buffer;
    struct ring_buffer     input_buffer;

    pin_t                  btn_pin;

    unsigned char          command[COMMAND_BUFFER_SIZE];
};
