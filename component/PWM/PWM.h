#pragma once

#include <IO.h>
#include <Timer.h>

#define PWM(instance, props, state) define(PWM, instance, _(props), _(state))

typedef struct
{
    unsigned int         frequency;
    unsigned char        duty_cycle;
} PWM_props_t;

typedef struct {
    io_handler *           io;
    void *                 pin;

    struct Timer          *timer;

    volatile unsigned long tick;      /* Last state changing timestamp */
    bool                   on_duty;   /* PWM state */
} PWM_state_t;

React_Header(PWM);
