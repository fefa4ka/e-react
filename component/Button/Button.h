#pragma once

#include <Timer.h>
#include <IO.h>

#define Button(instance, props) define(Button, instance, _(props), {0})
#define Button_isPressed(instance) React_State(Button, instance, pressed)

typedef struct
{
    io_handler *io;
    void       *pin;

    enum
    {
        BTN_PUSH_PULLUP,    /* Button pressed only when pushed */
        BTN_PUSH,
        BTN_TOGGLE_PULLUP,  /* Pressed state change after every push */
        BTN_TOGGLE
    } type;


    struct Timer          *timer;             /* Timestamp for bounce filtering */
    int                    bounce_delay_ms;

    void   (*onPress) (Component *instance);
    void (*onRelease) (Component *instance);
    void  (*onToggle) (Component *instance);  /* When pressed state changes */
} Button_props_t;

typedef struct
{
    bool          inverse;    /* Is pin operating in pullup configuration */
    bool          level;
    bool          pressed;    /* Is button pressed */
    unsigned long tick;       /* Timestamp when pin state changed */
} Button_state_t;

React_Header (Button);
