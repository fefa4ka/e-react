#include "Button.h"

willMount (Button)
{
    /* Setup pin as input */
    props->io->in (&props->pin);
    state->inverse
        = props->type == BTN_PUSH_PULLUP || props->type == BTN_TOGGLE_PULLUP;

    if(state->inverse) {
        props->io->pullup(&props->pin);
    }
}

shouldUpdate (Button)
{
    bool         level          = props->io->get (&props->pin);
    bool         is_level_equal = state->level == level;
    bool         state_level = state->inverse ? !state->level : state->level;
    unsigned int passed      = props->time->time_ms - state->tick;


    if (passed < 0) {
        passed = 65535 - state->tick + props->time->time_ms;
    }

    // Second check after bounce_delay_ms
    if (state->tick && passed >= props->bounce_delay_ms) {
        return true;
    }

    /* Already pressed */
    if (state_level == 1 && is_level_equal && state->pressed) {
        return false;
    }

    /* First high level detected */
    if (state_level == 0 && !is_level_equal) {
        return true;
    }


    /* Another checks after pressed */
    if ((props->type == BTN_PUSH_PULLUP || props->type == BTN_PUSH)
        && state->pressed && (level == 0 || state->inverse)) {
        // Push button unpressed after release
        return true;
    }
    
    state->level = level;
    return false;
}

willUpdate (Button)
{
    // Actual state
    state->level = props->io->get (&props->pin);
    bool         state_level = state->inverse ? !state->level : state->level;

    // Set initial tick to start count delay
    if (!state->tick && state_level) {
        state->tick = props->time->time_ms;
    }
}

release (Button)
{

    unsigned int passed  = props->time->time_ms - state->tick;
    bool         pressed = false;
    bool         state_level = state->inverse ? !state->level : state->level;

    if (props->type == BTN_TOGGLE_PULLUP
        || props->type == BTN_TOGGLE) {
        pressed = state->pressed;
    }

    if (state->tick && passed > props->bounce_delay_ms) {
        if (state_level) {
            if (props->type == BTN_TOGGLE_PULLUP
                || props->type == BTN_TOGGLE) {
                pressed = !state->pressed;
            } else {
                pressed = true;
            }
        }

        state->tick = 0;
    }

    if (state->pressed != pressed) {
        if (props->onToggle)
            props->onToggle (self);

        if (pressed) {
            if (props->onPress)
                props->onPress (self);
        } else {
            if (props->onRelease)
                props->onRelease (self);
        }

        state->pressed = pressed;
    }

}

didMount (Button) {}

didUnmount (Button) {}

didUpdate (Button) {}

React_Constructor (Button)
