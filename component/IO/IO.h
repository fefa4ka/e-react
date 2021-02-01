#pragma once

#include <component.h>
#include <stdbool.h>

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
} IO_blockProps;

typedef struct
{
    bool level;
} IO_blockState;

React_Header (IO);
#define IO(instance) component (IO, instance)
