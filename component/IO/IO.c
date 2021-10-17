#include "IO.h"

/**
 * \brief Initial pin mode configuration
 */
willMount(IO)
{
    /* Configure pin for operation in certain mode */
    if (state->mode == IO_INPUT) {
        state->io->in(state->pin);
    } else {
        state->io->out(state->pin);
    }
}

/**
 * \brief Updates everytime in input mode,
 *        otherwise updates if props changes
 */
shouldUpdate(IO)
{
    if (state->mode == IO_INPUT) {
        return true;
    }

    if (props->level != next_props->level) {
        return true;
    }

    return false;
}

willUpdate(IO) {}

/**
 * \brief Actual pin state configuration
 *        and callbacks triggering
 */
release(IO)
{
    if (state->mode == IO_OUTPUT) {
        /* Set pin level */
        if (props->level == IO_HIGH) {
            state->io->on(state->pin);
        } else {
            state->io->off(state->pin);
        }
    } else {
        bool level = state->io->get(state->pin);
        if (state->level != level) {
            state->level = level;

            if (state->onChange) {
                state->onChange(self);
            }

            if (level != 0 && state->onHigh) {
                state->onHigh(self);
            } else if (level == 0 && state->onLow) {
                state->onLow(self);
            }
        }
    }
}

/**
 * \brief First level reading
 */
didMount(IO)
{
    if (state->onChange) {
        state->onChange(self);
    }
}

didUpdate(IO) {}
