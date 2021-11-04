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
        if (lr_exists(props->buffer, lr_owner(*props->pins)))
            /* Data available */
            return true;

        /* No data available */
        return false;
    }

    if (!props->baudrate)
        return true;

    /* Change state on baudrate */
    // TODO: Optimize baudrate calculation
    uint16_t bit_duration_us    = 1e6 / props->baudrate;
    uint16_t next_tick          = props->clock->us;
    int16_t  probably_passed_us = next_tick - state->tick;
    if (probably_passed_us < 0)
        probably_passed_us = 65535 - state->tick + next_tick;

    if (state->clock && probably_passed_us >= bit_duration_us >> 1) {
        /* Clock tick fall */
        state->clock = false;
        if (props->clk_pin)
            props->io->off(props->clk_pin);
    }

    if (probably_passed_us >= bit_duration_us)
        return true;

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

    foreach_pins(pin, props->pins)
    {
        if (*mode == PIN_MODE_INPUT) {
            if (state->position == 8) {
                /* Write full byte from input */
                // TODO: if zero?
                if (*data)
                    lr_write(props->buffer, *data, lr_owner(*pin));
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
                lr_data_t cell_data = 0;
                if (lr_read(props->buffer, &cell_data, lr_owner(*pin))
                    == ERROR_NONE) {
                    sending = true;
                    *data   = (uint8_t)cell_data;
                    if (props->bit_order == BIT_ORDER_MSB)
                        *data = reverse(*data);
                }
            }
        }

        data++;
        mode++;
    }


    /* Callback with argument */
    if (sending && state->operating == false)
        if (props->onStart && props->onStart->method)
            props->onStart->method(self, props->onStart->argument);

    state->operating = sending;

    if (state->position == 8) {
        /* Clear session */
        state->tick      = 0;
        state->position  = 0;
        state->operating = false;

        if (props->clk_pin)
            /* Clock falling tick */
            props->io->off(props->clk_pin);

        if (props->onTransmitted && props->onTransmitted->method)
            props->onTransmitted->method(self, props->onTransmitted->argument);
    }
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
            if (bit_value(*data, state->position))
                props->io->on(*pin);
            else
                props->io->off(*pin);

            if (state->position == 7)
                *data = 0;
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
        /* Increment bit position */
        state->position++;

        /* Clock rise tick */
        state->clock = true;
        if (props->clk_pin)
            props->io->on(props->clk_pin);
    }
}

// TODO: Unmount Off every pin
