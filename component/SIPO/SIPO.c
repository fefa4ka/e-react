#include "SIPO.h"

void
SIPO_spacer (void *pin)
{
}

void
SIPO_on (void *pin)
{
    struct SIPO_pin *Pin = (struct SIPO_pin *)pin;
    SIPO_blockState *state = Instance_State (SIPO, (Component *)(Pin->bus));

    state->pins_state = bit_set (state->pins_state, Pin->number);
}

void
SIPO_off (void *pin)
{
    struct SIPO_pin *Pin = (struct SIPO_pin *)pin;
    SIPO_blockState *state = Instance_State (SIPO, (Component *)(Pin->bus));
    state->pins_state = bit_clear (state->pins_state, Pin->number);
}

void
SIPO_flip (void *pin)
{
    struct SIPO_pin *Pin = (struct SIPO_pin *)pin;
    SIPO_blockState *state = Instance_State (SIPO, (Component *)(Pin->bus));
    state->pins_state ^= bit_set (state->pins_state, Pin->number);
}

bool
SIPO_get (void *pin)
{
    struct SIPO_pin *Pin = (struct SIPO_pin *)pin;
    SIPO_blockState *state = Instance_State (SIPO, (Component *)(Pin->bus));
    return state->pins_state & (1 << (Pin->number));
}

io_handler SIPO_handler = { .in     = SIPO_spacer,
                            .out    = SIPO_spacer,
                            .on     = SIPO_on,
                            .off    = SIPO_off,
                            .flip   = SIPO_flip,
                            .get    = SIPO_get,
                            .pullup = SIPO_spacer };

static enum pin_mode SIPO_modes = OUTPUT;

void
SIPO_reset (void *instance, void *sipo)
{
    SIPO_blockState *state = Instance_State (SIPO, (Component *)sipo);

    state->reseting        = true;
}

willMount (SIPO)
{
    SIPO_blockState State
        = { .bitbanger       = React_Define_Component (Bitbang, NULL, NULL),
            .bitbanger_props = { .io       = props->io,
                                 .time     = props->time,
                                 .baudrate = props->baudrate,
                                 .pins     = state->pins,
                                 .clock    = props->clk_pin,
                                 .modes    = &SIPO_modes,
                                 .buffers  = state->buffers,
                                 .onStart  = &state->SIPO_reset },
            .pins            = { props->data_pin, NULL },
            .SIPO_reset      = { SIPO_reset, self },
            .buffer          = { NULL, 1 } };
    *state = State;

    state->bitbanger.state = &state->bitbanger_state;
    state->bitbanger.props = &state->bitbanger_props;

    state->buffer.data = &state->buffer_data;

    state->buffers[0] = &state->buffer;
    state->buffers[1] = NULL;

    state->pins[0] = props->data_pin;
    state->pins[1] = NULL;


    state->bitbanger.WillMount (&state->bitbanger);

    props->io->out (props->reset_pin);
    props->io->on (props->reset_pin);
}

shouldUpdate (SIPO)
{
    if (state->bitbanger.ShouldUpdate (&state->bitbanger,
                                       &state->bitbanger_props)) {
        return true;
    }

    if (state->pins_state != state->buffer_data
        && rb_length (&state->buffer) == 0) {
        return true;
    }
}

willUpdate (SIPO)
{
    state->bitbanger.WillUpdate (&state->bitbanger, &state->bitbanger_props);

    if (state->reseting) {
        props->io->off (props->reset_pin);
    }
}

release (SIPO)
{
    if (state->pins_state != state->buffer_data) {
        rb_write (&state->buffer, state->pins_state);
    }

    if (state->reseting) {
        state->reseting = false;
        props->io->on (props->reset_pin);
    }

    state->bitbanger.Release (&state->bitbanger);
}

didMount (SIPO) {}

didUnmount (SIPO) {}
didUpdate (SIPO) { state->bitbanger.DidUpdate (&state->bitbanger); }

React_Constructor (SIPO)
