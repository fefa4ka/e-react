
#pragma once

#include <Calendar.h>
#include <circular.h>
#include <component.h>
#include <hal.h>
#include <stdbool.h>

#define BITBANG_MAX_LINES 8

typedef struct
{
    io_handler *         io;
    struct rtc_datetime *time;

    unsigned int         baudrate;
    unsigned short       pins_number;
    void **              pins;
    enum pin_mode *      modes;
    struct ring_buffer **buffers;
    void *               clock;

    struct callback *onStart;
    struct callback *onTransmitted;
} Bitbang_blockProps;

typedef struct
{
    bool sending;

    unsigned char data[BITBANG_MAX_LINES];
    short         position;

    unsigned long tick;
} Bitbang_blockState;


React_Header (Bitbang);
#define Bitbang(instance) component (Bitbang, instance)
