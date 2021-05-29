#pragma once

#include <Timer.h>
#include <component.h>

#define Slider(instance, props) define(Slider, instance, _(props), {0})

typedef struct {
    unsigned int   value;
    unsigned short resolution;
    unsigned short scale;

    struct Timer *timer;

    void (*onIncrease)(Component *instance);
    void (*onDecrease)(Component *instance);
    void (*onChange)(Component *instance);
} Slider_props_t;

typedef struct {
    int            rpm;
    unsigned int   raw_value;
    unsigned short value;
    unsigned long  tick;
} Slider_state_t;

React_Header(Slider);
