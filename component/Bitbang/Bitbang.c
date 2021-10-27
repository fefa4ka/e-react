#include "Bitbang.h"

#define foreach_pins(pin, pins)                                                \
    void **pin;                                                               \
    for (pin = pins; *pin; pin++)

unsigned char reverse(unsigned char b)
{
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

    if (props->clk_pin) {
        props->io->out(props->clk_pin);
        props->io->off(props->clk_pin);
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
    if (state->data == 0) {
        // TODO: Check every OUTPUT pin
        if (lr_length_owned(props->buffer, lr_owner(*props->pins))) {
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
    uint8_t *data    = state->data;
    enum pin_mode *mode    = props->modes;
    bool           sending = state->sending;

    state->tick = props->clock->us;

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_INPUT) {
            if (state->position == -1) {
                /* Write full byte from input */
                if (*data) {
                    lr_write(props->buffer, *data, lr_owner(*pin));
                }
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
                if (lr_read(props->buffer, (lr_data_t *)data, lr_owner(*pin))
                    == ERROR_NONE) {
                    if (props->msb_first) {
                        *data = reverse(*data);
                    }
                    sending = true;
                }
            }
        }

        data++;
        mode++;
    }

    if (state->position == -1)
        state->position++;

    /* Callback with argument */
    if (sending && state->sending == false && props->onStart
        && props->onStart->method)
        props->onStart->method(self, props->onStart->argument);

    state->sending = sending;

    /* Clock tick fall */
    if (props->clk_pin)
        props->io->off(props->clk_pin);
}

/**
 * \brief     Set level for output lines
 */
release(Bitbang)
{
    uint8_t *data = state->data;
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

            if (props->clk_pin)
                /* Clock falling tick */
                props->io->off(props->clk_pin);

            if (props->onTransmitted && props->onTransmitted->method)
                props->onTransmitted->method(self,
                                             props->onTransmitted->argument);
        } else {
            /* Increment bit position */
            state->position++;

            /* Clock rise tick */
            if (props->clk_pin)
                props->io->on(props->clk_pin);
        }
    }
}

// TODO: Unmount Off every pin
