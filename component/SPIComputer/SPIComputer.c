#include "SPIComputer.h"

static void          SPI_init(void *bitbanger, void *spi);
static void          SPI_receive(void *bitbanger, void *spi_ptr);
static enum pin_mode SPI_modes[] = {PIN_MODE_OUTPUT, PIN_MODE_INPUT};

willMount(SPIComputer)
{
    Bitbang(bitbanger, _({
                           .io    = props->io,
                           .clock = props->clock,

                           .baudrate = props->baudrate,

                           .pins   = (void **)&props->bus,
                           .modes  = SPI_modes,
                           .buffer = props->buffer,

                           .clk_pin = props->bus.clk_pin,

                           .onStart       = &state->onStart,
                           .onTransmitted = &state->onReceive,
                       }));

    state->bitbanger = bitbanger;

    state->onStart.method   = SPI_init;
    state->onReceive.method = SPI_receive;
    state->onStart.argument = state->onReceive.argument = self;

    Stage_Component((Component *)&state->bitbanger, 0);
}

shouldUpdate(SPIComputer) { return Bitbang_shouldUpdate(&state->bitbanger, 0); }

willUpdate(SPIComputer) { Bitbang_willUpdate(&state->bitbanger, 0); }

release(SPIComputer) { Bitbang_release(&state->bitbanger); }

didMount(SPIComputer) {}

didUpdate(SPIComputer) { Bitbang_didUpdate(&state->bitbanger); }


#define lr_owner_chip_select(owner, value) ((owner << 4) | value)
#define lr_owner_callback(owner, pointer)                                      \
    ((owner & 0x0F) | (lr_owner(pointer) << 4))

static void SPI_init(void *bitbanger_ptr, void *spi_ptr)
{
    Bitbang_Component *    bitbanger  = (Bitbang_Component *)bitbanger_ptr;
    SPIComputer_Component *spi        = (SPIComputer_Component *)spi_ptr;
    lr_owner_t             copi_owner = lr_owner(spi->props.bus.copi_pin);

    if (lr_read(spi->props.buffer, (lr_data_t *)&spi->state.chip_select_pin,
                lr_owner_chip_select(copi_owner, *bitbanger->state.data))) {
        spi->state.chip_select_pin = NULL;
    }

    if (lr_read(spi->props.buffer, (lr_data_t *)&spi->state.callback,
                lr_owner_callback(copi_owner, *bitbanger->state.data))
        == ERROR_BUFFER_EMPTY) {
        spi->state.callback = NULL;
    }

    if (spi->state.chip_select_pin) {
        spi->props.io->out(spi->state.chip_select_pin);
        spi->props.io->on(spi->state.chip_select_pin);
    }
}

/* TODO: Architecture dependent masking */
void SPI_write(SPIComputer_Component *spi, unsigned char address,
               unsigned char value, void *chip_select_pin)
{
    lr_owner_t copi_owner = lr_owner(spi->props.bus.copi_pin);

    lr_write(spi->props.buffer, address, copi_owner);
    lr_write(spi->props.buffer, (lr_data_t)chip_select_pin,
             lr_owner_chip_select(copi_owner, address));
    lr_write(spi->props.buffer, value, copi_owner);
    lr_write(spi->props.buffer, (lr_data_t)chip_select_pin,
             lr_owner_chip_select(copi_owner, value));
}

void SPI_read(SPIComputer_Component *spi, unsigned char address,
              struct callback *callback, void *chip_select_pin)
{
    lr_owner_t copi_owner = lr_owner(spi->props.bus.copi_pin);

    SPI_write(spi, address, 0, chip_select_pin);
    lr_write(spi->props.buffer, (lr_data_t)callback,
             lr_owner_callback(copi_owner, callback));
}

static void SPI_receive(void *bitbanger_ptr, void *spi_ptr)
{
    lr_data_t              data      = 0;
    Bitbang_Component *    bitbanger = (Bitbang_Component *)bitbanger_ptr;
    SPIComputer_Component *spi       = (SPIComputer_Component *)spi_ptr;

    lr_read(spi->props.buffer, &data, lr_owner(spi->props.bus.cipo_pin));
    *bitbanger->state.data = data;

    if (spi->state.callback) {
        spi->state.callback->method(bitbanger->state.data,
                                    spi->state.callback->argument);
    }
    if (spi->state.chip_select_pin) {
        spi->props.io->off(spi->state.chip_select_pin);
    }
}
