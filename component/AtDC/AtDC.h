#pragma once

#include <stddef.h>
#include <component.h>
#include <hal.h> 


typedef struct
{
    adc_handler *adc;
    void *prescaler;
    void *channel;

    void (*onChange)(Component *instance);
} AtDC_blockProps;

typedef struct {
    unsigned short value;
    bool loading;
} AtDC_blockState;

React_Header(AtDC);

