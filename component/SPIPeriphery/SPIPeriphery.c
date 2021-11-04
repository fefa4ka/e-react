#include "SPIPeriphery.h"

willMount(SPIPeriphery)
{
    props->io->out(props->bus.cipo_pin);
    props->io->in(props->bus.copi_pin);
    props->io->in(props->bus.clk_pin);
}

shouldUpdate(SPIPeriphery)
{
    bool clk_level = props->io->get(props->bus.clk_pin);

    /* No communication */
    if (!clk_level && state->bit_position == 0) {
        return false;
    }

    /* Component not selected */
    if (props->bus.chip_select_pin
        && props->io->get(props->bus.chip_select_pin) == 0) {
        return false;
    }

    /* All done */
    if (state->clk_level == clk_level) {
        return false;
    }

    state->clk_level = clk_level;

    /* Clock tick */
    return true;
}

willUpdate(SPIPeriphery)
{
    /* Data for sending available */
    if (!state->sending && state->bit_position >= 8) {
        lr_data_t sending;
        lr_read(props->buffer, (lr_data_t *)&sending, lr_owner(self));
        state->sending = (uint8_t)sending;
    }
}

release(SPIPeriphery)
{
    uint8_t *pointer;
    uint8_t  position;

    if (state->bit_position < 8) {
        pointer  = &state->address;
        position = state->bit_position;
    } else if (state->bit_position >= 8) {
        pointer  = &state->data;
        position = state->bit_position - 8;
    }

    if (state->clk_level) {
        /* Receiving */
        if (props->io->get(props->bus.copi_pin))
            bit_set(*pointer, position);
        else
            bit_clear(*pointer, position);

    } else if (state->sending && state->bit_position >= 8) {
        /* Sending */
        if (bit_value(state->sending, position))
            props->io->on(props->bus.cipo_pin);
        else
            props->io->off(props->bus.cipo_pin);
    }
}

didMount(SPIPeriphery) {}

didUpdate(SPIPeriphery)
{
    void (*callback)(Component *) = NULL;

    if (state->clk_level && state->bit_position == 7 && props->onStart)
        callback = props->onStart;

    if (state->bit_position == 16) {
        state->bit_position = 0;
        state->sending      = 0;

        if (props->onReceive)
            callback = props->onReceive;

        props->io->off(props->bus.cipo_pin);
    } else if (state->clk_level) {
        state->bit_position++;
    }

    if (callback)
        (*callback)(self);
}
