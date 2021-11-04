#include "SIPO.h"

void SIPO_spacer(void *pin) {}

void SIPO_on(void *pin)
{
    struct SIPO_pin *Pin = (struct SIPO_pin *)pin;
    SIPO_Component * bus = (SIPO_Component *)Pin->bus;

    bit_set(bus->state.pins_state, Pin->number);
}

void SIPO_off(void *pin)
{
    struct SIPO_pin *Pin = (struct SIPO_pin *)pin;
    SIPO_Component * bus = (SIPO_Component *)Pin->bus;

    bit_clear(bus->state.pins_state, Pin->number);
}

void SIPO_flip(void *pin)
{
    struct SIPO_pin *Pin = (struct SIPO_pin *)pin;
    SIPO_Component * bus = (SIPO_Component *)Pin->bus;
    bit_flip(bus->state.pins_state, Pin->number);
}

bool SIPO_get(void *pin)
{
    struct SIPO_pin *Pin = (struct SIPO_pin *)pin;
    SIPO_Component * bus = (SIPO_Component *)Pin->bus;
    return bus->state.pins_state & (1 << (Pin->number));
}

io_handler SIPO_handler = {.in     = SIPO_spacer,
                           .out    = SIPO_spacer,
                           .on     = SIPO_on,
                           .off    = SIPO_off,
                           .flip   = SIPO_flip,
                           .get    = SIPO_get,
                           .pullup = SIPO_spacer};

static enum pin_mode SIPO_modes = PIN_MODE_OUTPUT;

void SIPO_reset(void *instance, void *sipo)
{
    SIPO_Component *bus = (SIPO_Component *)sipo;

    bus->state.reseting = true;
}

void SIPO_next(void *instance, void *sipo)
{
    SIPO_Component *bus = (SIPO_Component *)sipo;

    if (bus->state.pins_state != bus->state.released_state)
        if (lr_write(bus->props.buffer, bus->state.pins_state,
                     lr_owner(bus->props.bus.data_pin))
            == ERROR_NONE)
            bus->state.released_state = bus->state.pins_state;
}

willMount(SIPO)
{
    Bitbang(bitbanger, _({.io    = props->io,
                          .clock = props->clock,

                          .baudrate = props->baudrate,

                          .pins   = (void **)&props->bus,
                          .modes  = &SIPO_modes,
                          .buffer = props->buffer,

                          .clk_pin = props->bus.clk_pin,

                          .onStart       = &state->SIPO_reset,
                          .onTransmitted = &state->SIPO_next}));

    state->bitbanger = bitbanger;

    state->SIPO_reset.method   = SIPO_reset;
    state->SIPO_next.method    = SIPO_next;
    state->SIPO_reset.argument = state->SIPO_next.argument = self;

    Stage_Component((Component *)&state->bitbanger, 0);

    props->io->out(props->bus.reset_pin);
    props->io->on(props->bus.reset_pin);
}

shouldUpdate(SIPO) {
    if(state->bitbanger.state.operating) {
        return Bitbang_shouldUpdate(&state->bitbanger, 0);
    } else if(state->released_state != state->pins_state) {
        // TODO: maybe extra adds
        SIPO_next(&state->bitbanger, self);
    }

    return false;
}

willUpdate(SIPO)
{
    Bitbang_willUpdate(&state->bitbanger, 0);

    if (state->reseting)
        props->io->off(props->bus.reset_pin);
}

release(SIPO)
{
    if (state->reseting) {
        state->reseting = false;
        props->io->on(props->bus.reset_pin);
    }

    Bitbang_release(&state->bitbanger);
}

didMount(SIPO) {}

didUpdate(SIPO) { Bitbang_didUpdate(&state->bitbanger); }
