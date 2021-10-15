#include "Bitbang.h"
#include <common.h>

#define foreach_pins(pin, pins)                                                \
    pin_t **pin;                                                               \
    for (pin = pins; *pin; pin++)

unsigned char
reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;

   return b;
}
/**
 * \brief    Configure used pins for selected modes
 */
willMount(Bitbang)
{
    // setup pins
    enum pin_mode *mode = props->modes;

    if (props->clock) {
        props->io->out(props->clock);
        props->io->off(props->clock);
    }

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_INPUT)
            props->io->in(*pin);
        else
            props->io->out(*pin);


        mode++;
    }
}

/**
 * \brief    Updates if output data available
 *           or if next operation scheduled
 */
shouldUpdate(Bitbang)
{
    /* Component free for operation */
    if (*state->data == 0) {
        if (rb_length(*props->buffers)) {
            /* Data available */

            return true;
        }

        /* No data available */
        return false;
    }

    if (props->baudrate == 0) {
        return true;
    }

    // Change state on baudrate
    unsigned long bit_duration = 1000000 / props->baudrate;
    if (props->clock->us + props->clock->step_us - state->tick
        >= bit_duration) {

        return true;
    }


    return false;
}

/**
 * \brief    Prepare sending data and read input
 */
willUpdate(Bitbang)
{
    unsigned char *      data    = state->data;
    enum pin_mode *      mode    = props->modes;
    struct ring_buffer **buffer  = props->buffers;
    bool                 sending = state->sending;

    state->tick = props->clock->us;

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_INPUT) {
            if (state->position == -1) {
                /* Write full byte from input */
                if (*data)
                    rb_write(*buffer, *data);
            } else {
                /* Read bit */
                if (props->io->get(*pin))
                    bit_set(*data, state->position);
                else
                    bit_clear(*data, state->position);
            }
        } else {
            /* Read new byte for output */
            if (state->sending == false) {
                if (rb_read(*buffer, data) == ERROR_NONE) {
                    if (props->little_endian) {
                        *data = reverse(*data);
                    }
                    sending = true;
                }
            }
        }

        data++;
        mode++;
        buffer++;
    }

    if (state->position == -1)
        state->position++;

    /* Callback with argument */
    if (sending && state->sending == false && props->onStart
        && props->onStart->method)
        props->onStart->method(self, props->onStart->argument);

    state->sending = sending;

    /* Clock tick fall */
    if (props->clock)
        props->io->off(props->clock);
}

/**
 * \brief     Set level for output lines
 */
release(Bitbang)
{
    unsigned char *data = state->data;
    enum pin_mode *mode = props->modes;

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_OUTPUT) {
            if (bit_value(*data, state->position)) {
                props->io->on(*pin);
            } else {
                props->io->off(*pin);
            }

            if (state->position == 8) {
                *data = 0;
            }
        }
        data++;
        mode++;
    }
}

didMount(Bitbang) {}

/**
 * \brief    Clock ticks and transmitting finishing
 */
didUpdate(Bitbang)
{
    if (state->sending) {
        if (state->position == 8) {
            /* Clear session */
            state->sending  = false;
            state->position = -1;

            if (props->clock)
                /* Clock falling tick */
                props->io->off(props->clock);

            if (props->onTransmitted && props->onTransmitted->method)
                props->onTransmitted->method(self,
                                             props->onTransmitted->argument);
        } else {
            /* Increment bit position */
            state->position++;

            /* Clock rise tick */
            if (props->clock)
                props->io->on(props->clock);
        }
    }
}
