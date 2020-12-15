#include "IO.h"

static inline void
IO_setPinMode (IO_blockProps *props)
{
    if (props->mode == IO_INPUT) {
        props->io->in (props->pin);
    } else {
        props->io->out (props->pin);
    }
}

static inline void
IO_setPinState (IO_blockProps *props)
{
    if (props->level == IO_HIGH) {
        props->io->on (props->pin);
    } else {
        props->io->off (props->pin);
    }
}

willMount (IO) { IO_setPinMode (props); }

shouldUpdate (IO)
{
    if (props->mode == IO_INPUT) {
        return true;
    }

    if (props->level != nextProps->level || props->mode != nextProps->mode
        || props->io->in != nextProps->io->in) {
        return true;
    }

    return false;
}

willUpdate (IO)
{
    if (props->mode != nextProps->mode || props->io->in != nextProps->io->in) {
        IO_setPinMode (props);
    }
}

release (IO)
{
    if (props->mode == IO_OUTPUT) {
        IO_setPinState (props);
    } else {
        bool level = props->io->get (props->pin);
        if (state->level != level) {
            state->level = level;
            if (level != 0 && props->onHigh) {
                props->onHigh (self);
            }
            if (level == 0 && props->onLow) {
                props->onLow (self);
            }
            if (props->onChange) {
                props->onChange (self);
            }
        }
    }
}

didMount (IO)
{
    if (props->onChange) {
        props->onChange (self);
    }
}

didUnmount (IO) {}
didUpdate (IO) {}

React_Constructor (IO)
