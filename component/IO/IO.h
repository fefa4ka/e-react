#pragma once

#include <component.h>
#include <stdbool.h>

#define IO(instance) React_Define(IO, instance)
#define IO_new(instance, props) React_Define_WithProps(IO, instance, _(props))

typedef struct
{
    io_handler *io;
    void *      pin;

    enum
    {
        IO_OUTPUT,
        IO_INPUT
    } mode;

    enum
    {
        IO_LOW,
        IO_HIGH
    } level;

    void (*onChange) (Component *instance);
    void (*onLow) (Component *instance);
    void (*onHigh) (Component *instance);
} IO_props_t;

typedef struct
{
    bool level;
} IO_state_t;

React_Header (IO);
