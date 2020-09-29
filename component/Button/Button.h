#pragma once

#include <Calendar.h>
#include <IO.h>

typedef struct
{
    io_handler *io;
    void       *pin;

    enum
    {
        push,
        toggle
    } type;


    struct rtc_datetime    *time;
    int             bounce_delay_ms;

    void (*onPress) (Component *instance);
    void (*onRelease) (Component *instance);
    void (*onToggle) (Component *instance);
} Button_blockProps;

typedef struct
{
    bool          level;
    bool          pressed;
    unsigned long tick;
} Button_blockState;

React_Header (Button);
#define Button(instance) component (Button, instance)
