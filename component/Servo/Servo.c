#include "Servo.h"

willMount(Servo_block) {
    props->io->out(props->pin);

    state->on_duty = true;
    state->duty_cycle = 1000 + props->angle * 50 / 9;
    state->remain_time = props->speed * 1000 - state->duty_cycle;
}

shouldUpdate(Servo_block) {
    if(props->angle != nextProps->angle) {
        return true;
    }

    return false;
}

willUpdate(Servo_block) { 
    state->duty_cycle = 1000 + props->angle * 50 / 9;
    state->remain_time = props->speed * 1000 - state->duty_cycle;

    self->stage = released;
}

release(Servo_block) {
    AVRPin *pin = props->pin;
    if(pin->number == 5)  {
        log_pin(D, 5); 
    }

    unsigned int timeout_us = state->remain_time;
    if(state->on_duty) {
        props->io->off(props->pin);
        state->on_duty = false;
    } else {
        timeout_us = state->duty_cycle;
        props->io->on(props->pin);

        state->on_duty = true;
    }

    Scheduler_enqueue(props->scheduler, timeout_us, self->componentRelease, self);
}

didMount(Servo_block) { }

didUnmount(Servo_block) { }
didUpdate(Servo_block) { }

React_Constructor(Servo_block);
