#include "Bitbang.h"
#include <common.h>

#define foreach_pins(pin, pins)                                               \
    pin_t **pin;                                                              \
    for (pin = pins; *pin; pin++)

willMount (Bitbang)
{
    // setup pins
    enum pin_mode *mode = props->modes;

    if (props->clock)
        props->io->out (props->clock);
        props->io->off (props->clock);

    foreach_pins (pin, props->pins)
    {
        if (*mode == INPUT)
            props->io->in (*pin);
        else
            props->io->out (*pin);
        

        mode++;
    }
}

shouldUpdate (Bitbang)
{
    /* Component free for operation */
    if (*state->data == 0) {
        if (rb_length (*props->buffers)) {
            /* Data available */

            return true;
        }

        /* No data available */
        return false;
    }

    // Change state on baudrate
    unsigned long bit_duration = 1000000 / props->baudrate;
    if (props->time->time_us + props->time->step_us - state->tick
        >= bit_duration) {

        return true;
    }

    return false;
}

willUpdate (Bitbang)
{
    unsigned char *      data    = state->data;
    enum pin_mode *      mode    = props->modes;
    struct ring_buffer **buffer  = props->buffers;
    bool                 sending = state->sending;

    state->tick = props->time->time_us;

    foreach_pins (pin, props->pins)
    {
        if (*mode == INPUT) {
            if (state->position == -1) {
                /* Write full byte from input */
                if (*data)
                    rb_write (*buffer, *data);
            } else {
                /* Read bit */
                if (props->io->get (*pin))
                    bit_set (*data, state->position);
                else
                    bit_clear (*data, state->position);
            }
        } else {
            /* Read new byte for output */
            if (state->sending == false) {
                if(rb_read (*buffer, data) == eErrorNone) {
                    sending = true;

                    if(props->onStart)
                        props->onStart(self);
                }
            }
        }

        data++;
        mode++;
        buffer++;
    }

    if (state->position == -1) 
        state->position++;
    
    state->sending = sending;
    if (props->clock)
        props->io->off (props->clock);
}

release (Bitbang)
{
    unsigned char *data = state->data;
    enum pin_mode *mode = props->modes;

    foreach_pins (pin, props->pins)
    {
        if (*mode == OUTPUT) {
            if (bit_value (*data, state->position)) {
                props->io->on (*pin);
            } else {
                props->io->off (*pin);
            }

            if (state->position == 8) {
                *data = 0;
            }
        }
        data++;
        mode++;
    }
}

didMount(Bitbang) { }
didUnmount(Bitbang) { }


didUpdate (Bitbang)
{
    /* Increment bit position */
    if (state->sending) {
        if (state->position == 8) {
            /* clear session */
            state->sending  = false;
            state->position = -1;

            if (props->clock)
                props->io->off (props->clock);

            if (props->onTransmitted)
                props->onTransmitted (self);
        } else {
            state->position++;

            if (props->clock)
                props->io->on (props->clock);
        }
    }
}


React_Constructor(Bitbang)
