#pragma once

#include <IO.h>
#include <Time.h>

typedef struct {
    io_handler *io;
    void *pin;
    
    enum {
        push,
        toggle
    } type;
    
    
    rtc_datetime_t   *time;
    int  bounce_delay_ms;

    void (*onPress)(Component *instance);
    void (*onRelease)(Component *instance);
    void (*onToggle)(Component *instance);
} Button_blockProps;

typedef struct {
    bool level;
    bool pressed;
    unsigned long tick;
} Button_blockState;

Component Button_block(Button_blockProps *props, Button_blockState *state);
