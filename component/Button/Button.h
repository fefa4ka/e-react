#pragma once

#include <Calendar.h>
#include <IO.h>

typedef struct
{
    io_handler *io;
    pin_t      pin;

    enum
    {
        BTN_PUSH_PULLUP,
        BTN_PUSH_PULLDOWN,
        BTN_TOGGLE_PULLUP,
        BTN_TOGGLE_PULLDOWN
    } type;


    struct rtc_datetime    *time;
    int             bounce_delay_ms;

    void (*onPress) (Component *instance);
    void (*onRelease) (Component *instance);
    void (*onToggle) (Component *instance);
} Button_blockProps;

typedef struct
{
    bool          inverse;
    bool          level;
    bool          pressed;
    unsigned long tick;
} Button_blockState;

React_Header (Button);
#define Button(instance) component (Button, instance)
#define Button_State(instance, attribute) ((Button_blockState *)instance.state)->attribute
