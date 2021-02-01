#pragma once

#include <Bitbang.h>
#include <component.h>
#include <stdbool.h>

struct SIPO_pin
{
    Component *  bus;
    unsigned int number;
};

typedef struct
{
    io_handler *         io;
    struct rtc_datetime *time;

    unsigned int baudrate;

    pin_t *data_pin;
    pin_t *reset_pin;
    pin_t *clk_pin;
} SIPO_blockProps;

typedef struct
{
    Component          bitbanger;
    Bitbang_blockState bitbanger_state;
    Bitbang_blockProps bitbanger_props;

    bool                reseting;
    void *              pins[3];
    unsigned char       pins_state;
    unsigned char       buffer_data;
    struct ring_buffer  buffer;
    struct ring_buffer *buffers[2];
    struct callback     SIPO_reset;
} SIPO_blockState;

extern io_handler SIPO_handler;

React_Header (SIPO);
#define SIPO(instance) component (SIPO, instance)
