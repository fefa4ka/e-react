#include "unit.h"
#include <Bitbang.h>

#define BUFFER_SIZE 32

Clock(clk, &hw.timer, TIMESTAMP);

pin_t clk_pin  = hw_pin(D, 2);
pin_t mosi_pin = hw_pin(D, 3);
pin_t miso_pin = hw_pin(D, 4);

/* Bitbang SPI output */
unsigned char      input_buffer[BUFFER_SIZE];
struct ring_buffer spi_input_buffer = {input_buffer, BUFFER_SIZE};

unsigned char      output_buffer[BUFFER_SIZE];
struct ring_buffer spi_output_buffer = {output_buffer, BUFFER_SIZE};

pin_t *             spi_pins[]    = {&mosi_pin, &miso_pin, NULL};
struct ring_buffer *spi_buffers[] = {&spi_output_buffer, &spi_input_buffer};
enum pin_mode       spi_modes[]   = {PIN_MODE_OUTPUT, PIN_MODE_INPUT};
Bitbang(spi, _({
                 .io       = &hw.io,
                 .clock    = &clk.state.time,
                 .baudrate = 9600,
                 .msb_first= true,
                 .pins     = (void **)spi_pins,
                 .clk_pin  = &clk_pin,
                 .modes    = spi_modes,
                 .buffers  = spi_buffers
             }));

test(count)
{

    loop(clk, spi);
}

void count() {
    unsigned char number_order[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0 };
    unsigned char *string = "hello";

    rb_write_string(&spi_output_buffer, number_order);
    sleep(1);

    rb_write_string(&spi_output_buffer, string);
    sleep(1);
}

