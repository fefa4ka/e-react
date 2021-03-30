#include "OneWire.h"

willMount (OneWire) {}

shouldUpdate (OneWire)
{
    bool level   = state->level;
    state->level = props->io->get (props->pin);

    if (state->position) {
        /* Ongoing communiction */
        return true;
    }

    /* Component free for operation */
    if (state->sending == false && state->position == 0) {
        if (rb_length (&props->buffer)) {
            /* Data available
             * Ping bus */
            props->io->out (props->pin);
            props->io->off (props->pin);
            state->tick = props->time->time_us;

            return true;
        }

        /* No data available */
        return false;
    }

    /* On fall edge, if data receiving */
    if (level && level != state->level) {
        return true;
    }

    return false;
}

willUpdate (OneWire)
{
    /* Read new byte for output */
    if (state->sending == false && state->position == 0) {
        if (rb_read (&props->buffer, &state->data) == eErrorNone) {
            state->sending  = true;
            state->position = -1;
        }
    } else if (state->sending && state->position == -1
        && props->time->time_us - state->tick >= 60) {
        /* Release pin for bus user response lisening */
        state->tick = 0;
        props->io->in (props->pin);
        state->position = 0;
    }

    if (state->sending && state->position == 0) {
        if (props->io->get (props->pin) == 0) {
            /* Some device available */
            state->position = 1;
        }
    }

    if (!state->position) {
        self->stage = released;
    }
}

release (OneWire)
{
    /* Check if bus ready and pulled up */
    if (state->sending == false && props->io->get(props->pin)) {
        /* New bit initiation for 15 us */
        if (state->tick == 0) {
            state->tick = props->time->time_us;
            props->io->out (props->pin);
            props->io->off (props->pin);
        } else if (props->time->time_us - state->tick >= 15) {
            if (bit_value (state->data, (state->position - 1))) {
                props->io->on (props->pin);
            } else {
                props->io->off (props->pin);
            }

            state->tick    = props->time->time_us;
            state->sending = true;
        }
    } else {
        /* Release pin */
        props->io->in (props->pin);
        state->sending  = false;
        state->tick     = 0;
        state->position = (state->position + 1) % 8; // TODO: fast div && (1 << 3)?
    }
}

didMount (OneWire) {}

didUnmount (OneWire) {}
didUpdate (OneWire) {}

React_Constructor (OneWire)
