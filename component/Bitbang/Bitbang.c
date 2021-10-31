#include "Bitbang.h"

#define foreach_pins(pin, pins)                                                \
    void **pin;                                                                \
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
 *           or if next operation scheduled.
 *           Primary output data should be
 *           on the first pin.
 */
shouldUpdate(Bitbang)
{
    /* Component free for operation */
    if (!state->operating) {
        // TODO: Check every OUTPUT pin
        if (lr_length_owned(props->buffer, lr_owner(*props->pins))) {
            /* Data available */
            log_debug("pin=%x length=%d", lr_owner(*props->pins),
                      lr_length_owned(props->buffer, lr_owner(*props->pins)));
            return true;
        }

        /* No data available */
        return false;
    }

    if (!props->baudrate) {
        return true;
    }

    // Change state on baudrate
    uint16_t bit_duration_us = 1e6 / props->baudrate;
    uint16_t probably_passed_us
        = props->clock->us + props->clock->step_us - state->tick;
    if (probably_passed_us >= bit_duration_us) {
        log_debug("probably_passed_us=%d "
                  "bit_duration_us=%d",
                  probably_passed_us, bit_duration_us);
        return true;
    } else if(props->clk_pin && state->clock && probably_passed_us >= bit_duration_us / 2) {
        /* Clock tick fall */
        state->clock = false;
        props->io->off(props->clk_pin);
    }

    return false;
}

/**
 * \brief    Prepare sending data and read input
 *           Should be called with non zero clock->us
 */
willUpdate(Bitbang)
{
    uint8_t *      data    = state->data;
    enum pin_mode *mode    = props->modes;
    bool           sending = state->operating;

    state->tick = props->clock->us;

    log_debug("sending=%d output=%x position=%d baudrate=%d", state->operating,
              *state->data, state->position, props->baudrate);

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_INPUT) {
            if (-1 == state->position) {
                /* Write full byte from input */
                // TODO: if zero?
                if (*data) {
                    lr_write(props->buffer, *data, lr_owner(*pin));

                    log_debug("pin=%x read=%x", *pin, *data);
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
            if (!state->operating) {
                if (lr_read(props->buffer, (lr_data_t *)data, lr_owner(*pin))
                    == ERROR_NONE) {
                    if (props->bit_order == BIT_ORDER_MSB)
                        *data = reverse(*data);
                    sending = true;

                    log_debug("pin=%x banging=%x", lr_owner(*pin), *data);
                }
            }
        }

        data++;
        mode++;
    }

    if (state->position == -1)
        state->position++;

    /* Callback with argument */
    if (sending && state->operating == false && props->onStart
        && props->onStart->method)
        props->onStart->method(self, props->onStart->argument);

    state->operating = sending;

}

/**
 * \brief     Set level for output lines
 */
release(Bitbang)
{
    uint8_t *      data = state->data;
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
    if (state->operating) {
        if (state->position == 8) {
            /* Clear session */
            state->operating  = false;
            state->tick     = 0;
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

            state->clock = true;
        }
    }
}

// TODO: Unmount Off every pin
