#pragma once

#include <IO.h>
#include <Calendar.h>

typedef struct
{
    io_handler *         io;
    void *               pin;
    unsigned int         frequency;
    unsigned char        duty_cycle;
    struct rtc_datetime *time;
} PWM_blockProps;

typedef struct {
    volatile unsigned long tick;
    bool          on_duty;
} PWM_blockState;

React_Header(PWM);
#define PWM(instance) component (PWM, instance)
