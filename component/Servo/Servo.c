#include "Servo.h"
#include <common.h>


willMount(Servo) {
    props->io->out(props->pin);

    state->on_duty = true;
    state->duty_cycle = 1000 + props->angle * 50 / 9;
    state->remain_time = props->speed * 1000 - state->duty_cycle;
}

shouldUpdate(Servo) {
    if(props->angle != nextProps->angle
            || props->enabled != nextProps->enabled) {
        return true;
    }

    if(props->enabled && state->scheduled == false) {
        return true;
    }

    return false;
}

willUpdate(Servo) { 
    state->duty_cycle = 1000 + props->angle * 50 / 9;
    state->remain_time = props->speed * 1000 - state->duty_cycle;

    if(state->scheduled) {
        self->stage = released;
    }
}

release(Servo) {
    if(props->enabled) {
        if(state->on_duty) {
            props->io->off(props->pin);
            state->on_duty = false;
        } else {
            props->io->on(props->pin);

            state->on_duty = true;
        }

        bool scheduled = Scheduler_enqueue(
                props->scheduler, 
                state->on_duty 
                    ? state->duty_cycle
                    : state->remain_time, 
                self->componentRelease, self
        );

        state->scheduled = scheduled;
    } else {
        props->io->off(props->pin);
        state->scheduled = false;
    }
}

didMount(Servo) { }

didUnmount(Servo) { }
didUpdate(Servo) { }

React_Constructor(Servo);
