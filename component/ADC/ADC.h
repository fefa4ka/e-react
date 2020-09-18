
#ifndef blocks_ADC_h
#define blocks_ADC_h

#include <stddef.h>
#include <component.h>
#include <hal.h> 


typedef struct
{
    adc_handler *adc;
    void *prescaler;
    void *channel;

    void (*onChange)(Component *instance);
} ADC_blockProps;

typedef struct {
    unsigned short value;
    bool loading;
} ADC_blockState;

React_Header(ADC);
//Component ADC_block(ADC_blockProps *props, ADC_blockState *state);

#endif
