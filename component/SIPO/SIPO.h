#pragma once

#include <Bitbang.h>

#define SIPO(instance) React_Define(SIPO, instance)

struct SIPO_pin
{
    Component *bus;
    unsigned int number;
};

typedef struct
{
    io_handler *         io;
    struct Timer        *timer;

    unsigned int baudrate;

    pin_t *data_pin;
    pin_t *reset_pin;
    pin_t *clk_pin;
} SIPO_props_t;

typedef struct
{
    Bitbang_Component          bitbanger;

    bool                reseting;
    void *              pins[3];
    unsigned char       pins_state;
    unsigned char       buffer_data[2];
    struct ring_buffer  buffer;
    struct ring_buffer *buffers[2];
    struct callback     SIPO_reset;
} SIPO_state_t;

extern io_handler SIPO_handler;

React_Header (SIPO);
