#include "PWM.h"

willMount(PWM) {
    props->io->out(props->pin);
}

shouldUpdate(PWM) {
    if(props->time->time_us >= state->tick) {
        return true;
    } 

    return false;
}

willUpdate(PWM) { 
    if(state->on_duty) {
        state->tick = props->time->time_us + (1000000L / props->frequency / 255) * (255 - props->duty_cycle);
        state->on_duty = false;
    } else {
        state->tick = props->time->time_us + (1000000L / props->frequency / 255) * props->duty_cycle;
        state->on_duty = true;
    }
}

release(PWM) {
    if(state->on_duty) {
        props->io->on(props->pin);
    } else {
        props->io->off(props->pin);
    }
}

didMount(PWM) { }
didUnmount(PWM) { }

didUpdate(PWM) {
 
}


React_Constructor(PWM)
