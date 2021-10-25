#include <SPIComputer.h>

#define BUFFER_SIZE 32
struct lr_cell cells[BUFFER_SIZE] = {0};
struct linked_ring buffer = { .cells = cells, .size = BUFFER_SIZE };

Clock(clk, &hw.timer, TIMESTAMP);

pin_t copi_pin = hw_pin(D, 0);
pin_t cipo_pin = hw_pin(D, 1);
pin_t clk_pin = hw_pin(D, 2);
pin_t chip_select_pin = hw_pin(B, 0);

SPIComputer(spi, _({.io       = &hw.io,
                    .clock    = &clk.state.time,
                    .baudrate = 9600,
                    .buffer   = &buffer,
                    .bus      = {
                        .copi_pin = &copi_pin,
                        .cipo_pin = &cipo_pin,
                        .clk_pin  = &clk_pin,
                    }}));


void hello(void *args) {
    printf("hello world\n");
}

struct callback callback = { hello, 0 };
int main(void) {
    SPI_write(&spi, 'a', 'b', &chip_select_pin);
    SPI_read(&spi, 'c', &callback, &chip_select_pin);
    loop(clk, spi);
}
