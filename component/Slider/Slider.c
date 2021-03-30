#include "Slider.h"

willMount(Slider) {}

shouldUpdate(Slider)
{
    if (state->raw_value != props->value) {
        return true;
    }

    return false;
}

willUpdate(Slider)
{
    if (state->tick) {
        int passed = props->timer->ms - state->tick;
        int diff   = state->raw_value - props->value;

        /* Tick overflow */
        if (passed < 0) {
            passed = 65535 - state->tick + props->timer->ms;
        }

        state->rpm = 60000 / ((1 << props->resolution) * (diff / passed));
    }

    state->raw_value = props->value;
    state->tick      = props->timer->ms;
}

release(Slider)
{
    unsigned int value_before = state->value;

    state->value = props->scale * state->raw_value / (1 << props->resolution);

    if (state->value > value_before) {
        if (props->onRight)
            props->onIncrease(self);
    } else if (state->value < value_before) {
        if (props->onLeft)
            props->onDecrease(self);
    }

    if (state->value != value_before)
        if (props->onChange)
            props->onChange(self);
}

didMount(Slider) {}

didUpdate(Slider) {}
