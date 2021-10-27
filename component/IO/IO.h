#pragma once

#include <component.h>

#define IO(instance) React_Define(IO, instance)
#define IO_new(instance, state) React_Define_WithState(IO, instance, _(state))

typedef struct
{
    enum
    {
        IO_LOW,
        IO_HIGH
    } level;
} IO_props_t;

typedef struct
{
    bool level;

    io_handler *io;
    void *      pin;

    enum
    {
        IO_OUTPUT,
        IO_INPUT
    } mode;

    void (*onChange) (Component *instance);
    void (*onLow) (Component *instance);
    void (*onHigh) (Component *instance);
} IO_state_t;

React_Header (IO);
