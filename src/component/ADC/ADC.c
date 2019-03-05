#include "ADC.h"

willMount(ADC_block) {
    state->loading = false;
    props->adc->mount(NULL);
}

shouldUpdate(ADC_block) {
    return true;
}

willUpdate(ADC_block) {
    if(state->loading == false) {
        props->adc->selectChannel(props->channel);
        props->adc->startConvertion(props->channel);

        state->loading = true;
    }
}

release(ADC_block) {
    if(props->adc->isConvertionReady(props->channel)) {
        unsigned short value = props->adc->readConvertion(props->channel);

        state->loading = false;

        if (state->value != value) {
            state->value = value;
            if(props->onChange) {
                props->onChange(self);
            }
        }
    }
}

didMount(ADC_block) {
   
}

didUnmount(ADC_block) { }
didUpdate(ADC_block) { }

React_Constructor(ADC_block);
