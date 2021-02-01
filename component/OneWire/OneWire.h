#pragma once

#include <Calendar.h>
#include <circular.h>
#include <component.h>
#include <stdbool.h>

typedef struct
{
    io_handler *         io;
    struct rtc_datetime *time;
    void *               pin;
    struct ring_buffer   buffer;
} OneWire_blockProps;

typedef struct
{
    bool sending;
    unsigned char data;
    int  position;
    bool level;
    unsigned long tick;
} OneWire_blockState;

React_Header (OneWire);
#define OneWire(instance) component (OneWire, instance)
