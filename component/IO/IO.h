#pragma once 

#include <stdbool.h>
#include <component.h>

typedef struct
{
    io_handler *io;
    pin_t      pin;

    enum {
        IO_OUTPUT,
        IO_INPUT 
    } mode;

    enum {
        IO_LOW,
        IO_HIGH 
    } level;

    void (*onChange)(Component *instance);
    void (*onLow)(Component *instance); 
    void (*onHigh)(Component *instance);
} IO_blockProps;

typedef struct {
    bool level;
} IO_blockState;

React_Header(IO);
#define IO(instance) component(IO, instance)
