#pragma once

#include <stddef.h>
#include <component.h>

#undef ADC
#define ADC(instance, props) define(ADC, instance, _(props), {0})

typedef struct
{
    adc_handler     *adc;
    void            *prescaler;
    void            *channel;

    void (*onChange)(Component *instance);
} ADC_props_t;

typedef struct {
    unsigned int   value;
    bool           loading;
} ADC_state_t;

React_Header(ADC);
