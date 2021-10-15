#include <Bitbang.h>
#include <Button.h>

#define signal_pin  hw_pin(B, 2)
#define BUFFER_SIZE 8

pin_t clk_pin  = hw_pin(D, 2);
pin_t mosi_pin = hw_pin(D, 3);
pin_t miso_pin = hw_pin(D, 4);


Clock(clock, &hw.timer, TIMESTAMP);

/* Bitbang SPI output */
unsigned char      input_buffer[BUFFER_SIZE];
struct ring_buffer spi_input_buffer = {input_buffer, BUFFER_SIZE};

unsigned char       output_buffer[BUFFER_SIZE];
struct ring_buffer  spi_output_buffer = {output_buffer, BUFFER_SIZE};

pin_t *             spi_pins[]        = {&mosi_pin, &miso_pin, NULL};
struct ring_buffer *spi_buffers[]     = {&spi_output_buffer, &spi_input_buffer};
enum pin_mode       spi_modes[]       = {PIN_MODE_OUTPUT, PIN_MODE_INPUT};
Bitbang(spi, _({
                 .io       = &hw.io,
                 .clock    = &clock.state.time,
                 .baudrate = 9600,
                 .pins     = (void **) spi_pins,
                 .clk_pin  = &clk_pin,
                 .modes    = spi_modes,
                 .buffers  = spi_buffers,
             }));


/* Push button that increment counter on release */
pin_t        counter_pin   = hw_pin(D, 0);
unsigned int counter_index = 0;
void         send_number(Component *trigger)
{
    counter_index++;

    rb_write(&spi_output_buffer, counter_index);
}

Button(counter, _({
                    .io  = &hw.io,
                    .pin = &counter_pin,

                    .clock = &clock.state.time,

                    .type            = BUTTON_PUSH_PULLUP,
                    .bounce_delay_ms = 100,

                    .onRelease = send_number,
                }));


/* Indcation */
IO(led);
pin_t led_pin = hw_pin(B, 0);



int main(void)
{
    loop(clock, spi, counter)
    {
        apply(IO, led,
              _({
                  .io    = &hw.io,
                  .pin   = &led_pin,
                  .mode  = IO_OUTPUT,
                  .level = counter_index % 5 == 0,
              }));
    }
}

