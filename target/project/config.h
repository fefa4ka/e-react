#pragma once

#include <hal.h>
#include <circular.h>
#include <Calendar.h>

#define   VERSION      1
#define   BAUDRATE     9600

#define   COMMAND_BUFFER_SIZE  32
#define   BUFFER_SIZE          128 

struct device
{
    struct rtc_datetime    time;
    struct ring_buffer     output_buffer;
    struct ring_buffer     input_buffer;

    unsigned char          command[COMMAND_BUFFER_SIZE];
} ;
