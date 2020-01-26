#include "PWM.h"

willMount(PWM_block) {
    props->io->out(props->pin);
}

shouldUpdate(PWM_block) {
    if(props->time->time_us >= state->tick) {
        return true;
    } 

    return false;
}

willUpdate(PWM_block) { 
    if(state->on_duty) {
        state->tick = props->time->time_us + (1000000L / props->frequency / 255) * (255 - props->duty_cycle);
        state->on_duty = false;
    } else {
        state->tick = props->time->time_us + (1000000L / props->frequency / 255) * props->duty_cycle;
        state->on_duty = true;
    }
}

release(PWM_block) {
    if(state->on_duty) {
        props->io->on(props->pin);
    } else {
        props->io->off(props->pin);
    }
}

didMount(PWM_block) { }
didUnmount(PWM_block) { }

didUpdate(PWM_block) {
 
}


React_Constructor(PWM_block);
