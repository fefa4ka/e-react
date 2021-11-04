#include <SPIComputer.h>
#include <SPIPeriphery.h>

#define BUFFER_SIZE 16
struct lr_cell     cells[BUFFER_SIZE] = {0};
struct linked_ring buffer             = {cells, BUFFER_SIZE};

Clock(clk, &hw.timer, TIMESTAMP);

pin_t copi_pin        = hw_pin(COPI, 0);
pin_t cipo_pin        = hw_pin(CIPO, 0);
pin_t clk_pin         = hw_pin(CLK, 0);
pin_t chip_select_pin = hw_pin(CS, 0);

pin_t debug_pin = hw_pin(B, 1);

SPIComputer(spi, _({.io       = &hw.io,
                    .clock    = &clk.state.time,
                    .baudrate = 1200,
                    .buffer   = &buffer,
                    .bus      = {
                        .copi_pin = &copi_pin,
                        .cipo_pin = &cipo_pin,
                        .clk_pin  = &clk_pin,
                    }}));

void mirror_echo(Component *instance)
{
    SPIPeriphery_Component *mirror = (SPIPeriphery_Component *)instance;
    uint8_t address = mirror->state.address + 1;
    if(!address)
        address = 1;
    lr_write(&buffer, address, lr_owner(instance));
}

void mirror_receive(Component *instance)
{
    SPIPeriphery_Component *mirror = (SPIPeriphery_Component *)instance;
}

SPIPeriphery(mirror, _({
            .io = &hw.io,
            .buffer = &buffer,
            .bus      = {
                .copi_pin = &copi_pin,
                .cipo_pin = &cipo_pin,
                .clk_pin  = &clk_pin,
                .chip_select_pin = &chip_select_pin,
            },
            .onStart = mirror_echo,
            .onReceive = mirror_receive
            }));


const char world[] = "world";
extern struct callback callback;
void       hello(void *message, void *argument)
{
    uint8_t address = *(uint8_t *)message;

    hw.io.flip(&debug_pin);
    printf("hello %s | address: %x buffer: %d\n", (char *)argument, address, lr_length(&buffer));
    SPI_read(&spi, address, &callback, &chip_select_pin);
}

struct callback callback = {hello, world};

int main(void)
{
    SPI_write(&spi, 1, 2, &chip_select_pin);
    SPI_read(&spi, 0x01, &callback, &chip_select_pin);

    loop(clk, mirror, spi);
}
