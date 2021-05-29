#include "SPI.h"

static enum pin_mode SPI_modes[] = {PIN_MODE_OUTPUT, PIN_MODE_INPUT};

void SPI_write(Component *instance, unsigned char address, unsigned char value,
               void *chip_select_pin)
{
    SPI_props_t *      spi_props  = React_Instance_Props(SPI, instance);
    struct SPI_buffer *spi_buffer = spi_props->buffer;

    rb_write(&spi_buffer->output, address);
    spi_buffer->chip_select[spi_buffer->output.read] = chip_select_pin;
    rb_write(&spi_buffer->output, value);
    spi_buffer->chip_select[spi_buffer->output.read] = chip_select_pin;
}

void SPI_read(Component *instance, unsigned char address,
              struct callback *callback, void *chip_select_pin)
{
    SPI_props_t *      spi_props  = React_Instance_Props(SPI, instance);
    struct SPI_buffer *spi_buffer = spi_props->buffer;

    SPI_write(instance, address, 0, chip_select_pin);
    spi_buffer->callback[spi_buffer->output.read]    = *callback;
    spi_buffer->chip_select[spi_buffer->output.read] = chip_select_pin;
}


static void SPI_receive(void *bitbanger, void *spi)
{
    SPI_props_t *spi_props       = React_Instance_Props(SPI, (Component *)spi);
    unsigned int index           = spi_props->buffer->output.read;
    void *       chip_select_pin = spi_props->buffer->chip_select[index];

    if (chip_select_pin) {
        spi_props->io->out(chip_select_pin);
        spi_props->io->on(chip_select_pin);
    }
}

static void SPI_init(void *bitbanger, void *spi)
{
    SPI_props_t *      spi_props  = React_Instance_Props(SPI, (Component *)spi);
    SPI_state_t *      spi_state  = React_Instance_State(SPI, (Component *)spi);
    struct SPI_buffer *spi_buffer = spi_props->buffer;
    unsigned int       index      = spi_buffer->output.read;

    void *           chip_select_pin = spi_buffer->chip_select[index];
    struct callback *callback        = &spi_buffer->callback[index];
    unsigned char    data;

    rb_read(&spi_state->input_buffer, &data);

    if (callback->method) {
        callback->method(&data, callback->argument);
        spi_buffer->callback[index].method = NULL;
    }
    if (chip_select_pin) {
        spi_props->io->off(chip_select_pin);
        spi_buffer->chip_select[index] = NULL;
    }
}


willMount(SPI)
{
    SPI_state_t State = {
        .pins         = {props->mosi_pin, props->miso_pin, NULL},
        .buffers      = {&props->buffer->output, NULL},
        .input_buffer = {NULL, 2},
        .SPI_init     = {SPI_init, self},
        .SPI_receive  = {SPI_receive, self},
    };
    *state = State;

    Bitbang_props_t bitbang_props = {
        .io   = props->io,
        .time = props->time,

        .baudrate = props->baudrate,

        .pins    = state->pins,
        .modes   = SPI_modes,
        .buffers = state->buffers,

        .clock = props->clk_pin,

        .onStart       = &state->SPI_init,
        .onTransmitted = &state->SPI_receive,
    };

    state->bitbang_props = bitbang_props;

    Bitbang_build(&State.bitbanger, &state->bitbang_props,
                  &state->bitbang_state);

    state->input_buffer.data = state->input_buffer_data;
    state->buffers[1]        = &state->input_buffer;

    Bitbang_willMount(&state->bitbanger, &next_props);
}

shouldUpdate(SPI)
{
    if (Bitbang_shouldUpdate(&state->bitbanger, &state->bitbang_props)) {
        return true;
    }

    return false;
}

willUpdate(SPI)
{
    Bitbang_willUpdate(&state->bitbanger, &state->bitbang_props);
}

release(SPI) { Bitbang_release(&state->bitbanger); }

didMount(SPI) {}

didUnmount(SPI) {}
didUpdate(SPI) { Bitbang_didUpdate(&state->bitbanger); }

React_Constructor(SPI)
