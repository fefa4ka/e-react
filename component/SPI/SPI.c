#include "SPI.h"

static enum pin_mode SPI_modes[] = { OUTPUT, INPUT };

void
SPI_write (Component *instance, unsigned char address, unsigned char value,
           void *chip_select_pin)
{
    SPI_blockProps *   spi_props  = Instance_Props (SPI, instance);
    struct SPI_buffer *spi_buffer = spi_props->buffer;

    rb_write (&spi_buffer->output, address);
    spi_buffer->chip_select[spi_buffer->output.read] = chip_select_pin;
    rb_write (&spi_buffer->output, value);
    spi_buffer->chip_select[spi_buffer->output.read] = chip_select_pin;
}

void
SPI_read (Component *instance, unsigned char address,
          struct callback *callback, void *chip_select_pin)
{
    SPI_blockProps *   spi_props  = Instance_Props (SPI, instance);
    struct SPI_buffer *spi_buffer = spi_props->buffer;

    SPI_write (instance, address, 0, chip_select_pin);
    spi_buffer->callback[spi_buffer->output.read]    = *callback;
    spi_buffer->chip_select[spi_buffer->output.read] = chip_select_pin;
}


static void
SPI_receive (void *bitbanger, void *spi)
{
    SPI_blockProps *spi_props       = Instance_Props (SPI, (Component *)spi);
    unsigned int    index           = spi_props->buffer->output.read;
    void *          chip_select_pin = spi_props->buffer->chip_select[index];

    if (chip_select_pin) {
        spi_props->io->out (chip_select_pin);
        spi_props->io->on (chip_select_pin);
    }
}

static void
SPI_init (void *bitbanger, void *spi)
{
    SPI_blockProps *   spi_props  = Instance_Props (SPI, (Component *)spi);
    SPI_blockState *   spi_state  = Instance_State (SPI, (Component *)spi);
    struct SPI_buffer *spi_buffer = spi_props->buffer;
    unsigned int       index      = spi_buffer->output.read;

    void *           chip_select_pin = spi_buffer->chip_select[index];
    struct callback *callback        = &spi_buffer->callback[index];
    unsigned char    data;
    rb_read (&spi_state->input_buffer, &data);

    if (callback->method) {
        callback->method (&data, callback->argument);
        spi_buffer->callback[index].method = NULL;
    }
    if (chip_select_pin) {
        spi_props->io->off (chip_select_pin);
        spi_buffer->chip_select[index] = NULL;
    }
}


willMount (SPI)
{
    SPI_blockState State
        = { .bitbanger    = React_Define_Component (Bitbang, NULL, NULL),
            .pins         = { props->mosi_pin, props->miso_pin, NULL },
            .buffers      = { &props->buffer->output, NULL },
            .input_buffer = { NULL, 2 },
            .SPI_init     = { SPI_init, self },
            .SPI_receive  = { SPI_receive, self } };
    *state                 = State;

    state->bitbanger.state = &state->bitbang_state;

    Bitbang_blockProps bitbang_props
        = { .io            = props->io,
            .time          = props->time,
            .baudrate      = props->baudrate,
            .pins          = state->pins,
            .clock         = props->clk_pin,
            .modes         = SPI_modes,
            .buffers       = state->buffers,
            .onStart       = &state->SPI_init,
            .onTransmitted = &state->SPI_receive };
    state->bitbang_props = bitbang_props;
    state->bitbanger.props = &state->bitbang_props;

    state->input_buffer.data = state->input_buffer_data;
    state->buffers[1]        = &state->input_buffer;

    state->bitbanger.WillMount (&state->bitbanger);
}

shouldUpdate (SPI)
{
    if (state->bitbanger.ShouldUpdate (&state->bitbanger,
                                       &state->bitbang_props)) {
        return true;
    }
}

willUpdate (SPI)
{
    state->bitbanger.WillUpdate (&state->bitbanger, &state->bitbang_props);
}

release (SPI) { state->bitbanger.Release (&state->bitbanger); }

didMount (SPI) {}

didUnmount (SPI) {}
didUpdate (SPI) { state->bitbanger.DidUpdate (&state->bitbanger); }

React_Constructor (SPI)
