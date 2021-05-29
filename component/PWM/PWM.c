#include "PWM.h"

/**
 * \brief    Configure pin as output
 */
willMount(PWM) {
    state->io->out(state->pin);
}

/**
 * \brief    Update pin level in desire time
 */
shouldUpdate(PWM) {
    if(state->timer->us >= state->tick) {
        return true;
    } 

    return false;
}

/**
 * \brief    Time of next pin level change depends
 *           on frequency and duty_cycle
 */
willUpdate(PWM) { 
    if(state->on_duty) {
        state->tick = state->timer->us + (1000000L / props->frequency / 255) * (255 - props->duty_cycle);
        state->on_duty = false;
    } else {
        state->tick = state->timer->us + (1000000L / props->frequency / 255) * props->duty_cycle;
        state->on_duty = true;
    }
}

/**
 * \brief     Change pin level
 */
release(PWM) {
    if(state->on_duty) {
        state->io->on(state->pin);
    } else {
        state->io->off(state->pin);
    }
}

didMount(PWM) {}
didUpdate(PWM) {}
