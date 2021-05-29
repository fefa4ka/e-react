
#pragma once

#include <IO.h>
#include <Scheduler.h>

#define Servo(instance, props, state)                                          \
    define(Servo, instance, _(props), _(state))

typedef struct {
    unsigned char speed;    /* PWM period in ms disabled if 0 */
    unsigned int  angle;    /* Servo position */
} Servo_props_t;

typedef struct {
    io_handler *         io;
    void *               pin;
    Scheduler_Component *scheduler;

    unsigned int duty_cycle;
    unsigned int remain_time;
    enum {
        SERVO_STANDY,
        SERVO_ON_DUTY,
        SERVO_ON_OFFSET
    } on_duty;
} Servo_state_t;


React_Header(Servo);
