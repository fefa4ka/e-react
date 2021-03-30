#include "Encoder.h"

willMount(Encoder)
{
    /* Setup pin as input */
    props->io->in(props->pin_a);
    props->io->in(props->pin_b);
}

shouldUpdate(Encoder)
{
    if (state->level_a != props->io->get(props->pin_a))
        return true;

    if (state->level_b != props->io->get(props->pin_b))
        return true;

    return false;
}

willUpdate(Encoder)
{
    bool level_a = props->io->get(props->pin_a);
    if (state->level_a != level_a) {
        state->level_a = level_a;
        state->tick_a  = props->timer->ms;

        /* Handle timer overflow */
        if(state->tick_b > state->tick_a) {
            state->tick_b = 0;
        }

        return;
    }

    bool level_b = props->io->get(props->pin_b);
    if (state->level_b != level_b) {
        state->level_b = level_b;
        state->tick_b  = props->timer->ms;

        if(state->tick_a > state->tick_b) {
            state->tick_a = 0;
        }

        return;
    }
}

release(Encoder)
{
    /* Measure diff between last a and b changes */
    bool is_right_turn = false;
    long passed        = state->tick_a - state->tick_b;

    if (state->level_a > state->level_b) {
        if (passed > 0) {
            is_right_turn = true;
        } 
    } else {
        if (passed < 0) {
            is_right_turn = true;
        }
    }

    if (state->tick_a && state->tick_b) {
        if(passed < 0) {
            passed *= -1;
        }

        state->rpm = 60000 / (props->resolution * passed);
    }

    if (is_right_turn) {
        if (props->onRight)
            props->onRight(self);
    } else {
        state->rpm *= -1;

        if (props->onLeft)
            props->onLeft(self);
    }

    if (props->onTurn)
        props->onTurn(self);
}

didMount(Encoder) {}

didUpdate(Encoder) {}
