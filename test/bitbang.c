#include "unit.h"
#include <Bitbang.h>

#define BUFFER_SIZE 32
#define buffer_size(owner)                                                     \
    lr_length_owned(&buffer, lr_owner(owner))

struct lr_cell     cells[BUFFER_SIZE] = {0};
struct linked_ring buffer             = {cells, BUFFER_SIZE};

Clock(clk, &hw.timer, TIMESTAMP);

pin_t clk_pin  = hw_pin(D, 2);
pin_t copi_pin = hw_pin(D, 3);
pin_t cipo_pin = hw_pin(D, 4);

/* Bitbang SPI output */
pin_t *       spi_pins[]  = {&copi_pin, &cipo_pin, NULL};
enum pin_mode spi_modes[] = {PIN_MODE_OUTPUT, PIN_MODE_INPUT};
Bitbang(spi, _({.io        = &hw.io,
                .clock     = &clk.state.time,
                .baudrate  = 9600,
                .bit_order = BIT_ORDER_MSB,
                .pins      = (void **)spi_pins,
                .clk_pin   = &clk_pin,
                .modes     = spi_modes,
                .buffer    = &buffer}));

test(count) { loop(clk, spi); }

void count()
{
    unsigned char number_order[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0};

    test_assert(buffer_size(&copi_pin) == 0,
                "COPI buffer should be empty, but %d", buffer_size(&copi_pin));
    test_assert(buffer_size(&cipo_pin) == 0,
                "CIPO buffer should be empty, but %d", buffer_size(&cipo_pin));

    hw.io.on(&cipo_pin);
    lr_write_string(&buffer, number_order, lr_owner(&copi_pin));
    sleep(1);

    test_assert(buffer_size(&copi_pin) == 0,
                "COPI buffer should be empty, but %d", buffer_size(&copi_pin));
    test_assert(buffer_size(&cipo_pin) == strlen(number_order),
                "CIPO buffer should be %d, but %d", strlen(number_order),
                buffer_size(&cipo_pin));
}
