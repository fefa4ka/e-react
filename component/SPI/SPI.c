#include "SPI.h"

static enum pin_mode SPI_modes[] = { OUTPUT, INPUT };

void
SPI_write (Component *instance, unsigned char address, unsigned char value,
           pin_t *chip_select_pin)
{
    SPI_blockProps *spi_props = (SPI_blockProps *)instance->props;
    unsigned int    index     = spi_props->buffer.output.read;

    rb_write (&spi_props->buffer.output, address);
    spi_props->buffer.chip_select[spi_props->buffer.output.read]
        = chip_select_pin;
    rb_write (&spi_props->buffer.output, value);
    spi_props->buffer.chip_select[spi_props->buffer.output.read]
        = chip_select_pin;
}

void
SPI_read (Component *instance, unsigned char address,
          struct callback *callback, pin_t *chip_select_pin)
{
    SPI_blockProps *spi_props = (SPI_blockProps *)instance->props;

    SPI_write (instance, address, 0, chip_select_pin);
    spi_props->buffer.callback[spi_props->buffer.output.read] = *callback;
    spi_props->buffer.chip_select[spi_props->buffer.output.read]
        = chip_select_pin;
}


static void
SPI_init (Component *instance)
{
    SPI_blockProps *spi_props       = (SPI_blockProps *)instance->props;
    unsigned int    index           = spi_props->buffer.output.read;
    pin_t *         chip_select_pin = spi_props->buffer.chip_select[index];

    if (chip_select_pin) {
        spi_props->io->out (chip_select_pin);
        spi_props->io->on (chip_select_pin);
    }
}

static void
SPI_receive (Component *instance)
{
    SPI_blockProps *spi_props = (SPI_blockProps *)instance->props;
    SPI_blockState *spi_state = (SPI_blockState *)instance->state;
    unsigned int    index     = spi_props->buffer.output.read;

    pin_t *chip_select_pin         = spi_props->buffer.chip_select[index];
    struct callback *callback = &spi_props->buffer.callback[index];
    unsigned char         data;
    rb_read (&spi_state->input_buffer, &data);

    if (callback->method) {
        callback->method (&data, callback->argument);
        spi_props->buffer.callback[index].method = NULL;
    }
    if (chip_select_pin) {
        spi_props->io->off (chip_select_pin);
        spi_props->buffer.chip_select[index] = NULL;
    }
}


willMount (SPI)
{
    Component bitbanger = React_Define_Component (Bitbang, &props->bitbang, &state->bitbang);
    state->bitbanger = bitbanger;

    Bitbang_blockProps bitbang_props
        = { .io            = props->io,
            .time          = props->time,
            .baudrate      = props->baudrate,
            .pins          = state->pins,
            .clock         = props->clk_pin,
            .modes         = SPI_modes,
            .buffers       = state->buffers,
            .onStart       = SPI_init,
            .onTransmitted = SPI_receive };
    props->bitbang = bitbang_props;

    state->pins[0] = props->mosi_pin;
    state->pins[1] = props->miso_pin;
    state->pins[2] = NULL;

    state->buffers[0] = &props->buffer.output;
    state->buffers[1] = &state->input_buffer;

    state->bitbanger.WillMount (&state->bitbanger);
}

shouldUpdate (SPI)
{
    if (state->bitbanger.ShouldUpdate (&state->bitbanger,
                                       &nextProps->bitbang)) {
        return true;
    }
}

willUpdate (SPI)
{
    state->bitbanger.WillUpdate (&state->bitbanger, &nextProps->bitbang);
}

release (SPI) { state->bitbanger.Release (&state->bitbanger); }

didMount (SPI) {}

didUnmount (SPI) {}
didUpdate (SPI) { state->bitbanger.DidUpdate (&state->bitbanger); }

React_Constructor (SPI)
