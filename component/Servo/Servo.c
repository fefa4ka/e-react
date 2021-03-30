#include "Servo.h"

static unsigned int g_seed = 31337;

inline int
fast_random ()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

willMount(Servo) {
    state->io->out(state->pin);

    state->on_duty = true;
    state->duty_cycle = 1000 + props->angle * 50 / 9;
    state->remain_time = props->speed * 1000 - state->duty_cycle;
}

shouldUpdate(Servo) {
    if(props->angle != next_props->angle
            || props->speed != next_props->speed)
        return true;

    if(props->speed && state->on_duty == SERVO_STANDY)
        return true;

    return false;
}

willUpdate(Servo) { 
    state->duty_cycle = 1000 + props->angle * 50 / 9;
    state->remain_time = props->speed * 1000 - state->duty_cycle;

    if(state->on_duty != SERVO_STANDY)
        self->stage = REACT_STAGE_RELEASED;

    if(props->speed == 0 && next_props->speed) {
        Servo_willMount(self, next_props);
    }
}

release(Servo) {
    if(props->speed) {
        if(state->on_duty == SERVO_ON_DUTY) {
            state->io->off(state->pin);
            state->on_duty = SERVO_ON_OFFSET;
        } else {
            state->io->on(state->pin);

            state->on_duty = SERVO_ON_DUTY;
        }

        bool scheduled = Scheduler_enqueue(
                state->scheduler, 
                (state->on_duty 
                    ? state->duty_cycle
                    : state->remain_time) 
                + (state->on_duty
                    ? 0 
                    : fast_random()), 
                self->Release, self
        );
    } else {
        state->io->off(state->pin);
        state->on_duty = SERVO_STANDY;
    }
}

didMount(Servo) {}
didUpdate(Servo) {}

