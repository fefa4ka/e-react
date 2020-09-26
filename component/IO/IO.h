#pragma once 

#include <stdbool.h>
#include <component.h>

typedef struct
{
    io_handler *io;
    void *pin;

    enum {
        output,
        input
    } mode : 4;

    enum {
        low,
        high
    } level : 4;

    void (*onChange)(Component *instance);
    void (*onLow)(Component *instance); 
    void (*onHigh)(Component *instance);
} IO_blockProps;

typedef struct {
    bool level;
} IO_blockState;

React_Header(IO);
#define IO(instance) component(IO, instance)
