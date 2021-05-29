#include "ADC.h"

/*
 * \brief ADC hardware initialization
 */
willMount(ADC) {
    state->loading = false;
    props->adc->mount(NULL);
}

/**
 * \brief   We always want update values
 */
shouldUpdate(ADC) {
    return true;
}

/**
 * \brief   Convertion initialization
 */
willUpdate(ADC) {
    if(state->loading == false) {
        props->adc->selectChannel(props->channel);
        props->adc->startConvertion(props->channel);

        state->loading = true;
    }
}

/**
 * \brief    Read converted value to state
 */
release(ADC) {
    if(props->adc->isConvertionReady(props->channel)) {
        unsigned short value = props->adc->readConvertion(props->channel);

        state->loading = false;

        if (state->value != value) {
            state->value = value;

            if(props->onChange) props->onChange(self);
        }
    }
}

didMount(ADC) {}

didUpdate(ADC) {}

