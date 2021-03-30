#include "IO.h"

/**
 * \brief Initial pin mode configuration
 */
willMount(IO) {
    IO_willUpdate(self, next_props);
}

/**
 * \brief Updates everytime in input mode,
 *        otherwise updates if props changes
 */
shouldUpdate(IO)
{
    if (props->mode == IO_INPUT) {
        return true;
    }

    if (props->level != next_props->level || props->mode != next_props->mode
        || props->io->in != next_props->io->in || props->pin != next_props->pin) {
        return true;
    }

    return false;
}

/**
 * \brief Configure pin mode if needed
 */
willUpdate(IO)
{
    if (props->mode != next_props->mode
        || props->io->in != next_props->io->in 
            || props->pin != next_props->pin 
                || self->stage == REACT_STAGE_DEFINED) {
        /* Configure pin for operation in certain mode */
        if (props->mode == IO_INPUT) {
            props->io->in(next_props->pin);
        } else {
            props->io->out(next_props->pin);
        }
    }
}

/**
 * \brief Actual pin state configuration
 *        and callbacks triggering
 */
release(IO)
{
    if (props->mode == IO_OUTPUT) {
        /* Configure pin level */
        if (props->level == IO_HIGH) {
            props->io->on(props->pin);
        } else {
            props->io->off(props->pin);
        }
    } else {
        bool level = props->io->get(props->pin);
        if (state->level != level) {
            state->level = level;

            if (props->onChange) {
                props->onChange(self);
            }

            if (level != 0 && props->onHigh) {
                props->onHigh(self);
            } else if (level == 0 && props->onLow) {
                props->onLow(self);
            }
        }
    }
}

/**
 * \brief First level reading
 */
didMount(IO)
{
    if (props->onChange) {
        props->onChange(self);
    }
}

didUpdate(IO) {}

