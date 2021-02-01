#include "AtDC.h"

willMount(AtDC) {
    state->loading = false;
    props->adc->mount(NULL);
}

shouldUpdate(AtDC) {
    return true;
}

willUpdate(AtDC) {
    if(state->loading == false) {
        props->adc->selectChannel(props->channel);
        props->adc->startConvertion(props->channel);

        state->loading = true;
    }
}

release(AtDC) {
    if(props->adc->isConvertionReady(props->channel)) {
        unsigned short value = props->adc->readConvertion(props->channel);

        state->loading = false;

        if (state->value != value) {
            state->value = value;

            if(props->onChange) props->onChange(self);
        }
    }
}

didMount(AtDC) {
   
}

didUnmount(AtDC) { }
didUpdate(AtDC) { }

React_Constructor(AtDC)
