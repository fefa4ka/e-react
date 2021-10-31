#include <SPIComputer.h>
#include <SPIPeriphery.h>

#define BUFFER_SIZE 32
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
                    .baudrate = 600,
                    .buffer   = &buffer,
                    .bus      = {
                        .copi_pin = &copi_pin,
                        .cipo_pin = &cipo_pin,
                        .clk_pin  = &clk_pin,
                    }}));

void mirror_echo(Component *instance) {
    SPIPeriphery_Component *mirror = (SPIPeriphery_Component *)instance;
    printf("addr: %x\n", mirror->state.address);
}
void mirror_receive(Component *instance) {
    SPIPeriphery_Component *mirror = (SPIPeriphery_Component *)instance;
    printf("data: %x\n", mirror->state.data);
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
void       hello(void *message, void *argument)
{
    hw.io.flip(&debug_pin);
    printf("hello %s %d\n", (char *)argument, *(char *)message);
}

struct callback callback = {hello, world};

int             main(void)
{

    SPI_write(&spi, 1, 2, &chip_select_pin);
    SPI_read(&spi, 'c', &callback, &chip_select_pin);

    SPI_write(&spi, 'a', 'b', &chip_select_pin);
    SPI_read(&spi, 'c', &callback, &chip_select_pin);
    lr_write(&buffer, 0x63, lr_owner(&cipo_pin));
    printf("length=%d\n", lr_length_owned(&buffer, lr_owner(&cipo_pin)));
    printf("copi=%x cipo=%x clk=%x\n", lr_owner(&copi_pin), lr_owner(&cipo_pin),
              lr_owner(&clk_pin));
    loop(clk, mirror, spi);
}
