#pragma once

#include <Scheduler.h>
#include <AtDC.h>
#include <Button.h>
#include <Scheduler.h>
#include <Servo.h>

#define   VERSION      1

enum sensor_channel_e {
    potentiomenter = 0,
    solar_panel = 1
};

struct device
{
    struct rtc_datetime    time;
    struct events_queue    scheduler; 

    enum sensor_channel_e  sensor;
    unsigned short         thrust;
    unsigned short         angle;

    bool                   motors_enabled;
    
    pin_t                  left_actuator_pin;
    pin_t                  right_actuator_pin;
    pin_t                  engine_pin;

    pin_t                  switcher_pin;
} ;
