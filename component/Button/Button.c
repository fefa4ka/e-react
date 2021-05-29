#include "Button.h"

/**
 * \brief  Configure pins for input and pullup if needed
 */
willMount(Button)
{
    /* Setup pin as input */
    props->io->in(props->pin);
    state->inverse
        = props->type == BTN_PUSH_PULLUP || props->type == BTN_TOGGLE_PULLUP;

    if (state->inverse) {
        props->io->pullup(props->pin);
    }
}

/**
 * \brief  Check if level doesn't change during bounce_delay_ms
 *         or check if push button released
 */
shouldUpdate(Button)
{
    bool level          = props->io->get(props->pin);
    bool is_level_equal = state->level == level;
    bool state_level    = state->inverse ? !state->level : state->level;
    int  passed         = props->timer->ms - state->tick;

    /* Tick overflow */
    if (passed < 0) {
        passed = 65535 - state->tick + props->timer->ms;
    }

    /* Second check after bounce_delay_ms */
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
        /* Push button unpressed after release */
        return true;
    }

    state->level = level;
    return false;
}

/**
 * \brief            Prepare button internal states
 */
willUpdate(Button)
{
    /* Actual state reading */
    state->level     = props->io->get(props->pin);
    bool state_level = state->inverse ? !state->level : state->level;

    /* Set initial tick to start delay counting */
    if (!state->tick && state_level) {
        state->tick = props->timer->ms;
    }
}

/**
 * \brief            Button pressed state logic
 */
release(Button)
{
    int  passed      = props->timer->ms - state->tick;
    bool pressed     = false;
    bool state_level = state->inverse ? !state->level : state->level;


    /* Tick overflow */
    if (passed < 0) {
        passed = 65535 - state->tick + props->timer->ms;
    }

    /* Cache button state for toggling */
    if (props->type == BTN_TOGGLE_PULLUP || props->type == BTN_TOGGLE) {
        pressed = state->pressed;
    }

    /* Change state if bounce filtered */
    if (state->tick && passed > props->bounce_delay_ms) {
        if (state_level) {
            if (props->type == BTN_TOGGLE_PULLUP || props->type == BTN_TOGGLE) {
                pressed = !state->pressed;
            } else {
                pressed = true;
            }
        }

        state->tick = 0;
    }

    /* Callback triggering */
    if (state->pressed != pressed) {
        if (pressed) {
            if (props->onPress)
                props->onPress(self);
        } else {
            if (props->onRelease)
                props->onRelease(self);
        }

        if (props->onToggle)
            props->onToggle(self);

        state->pressed = pressed;
    }
}

didMount(Button) {}
didUpdate(Button) {}
