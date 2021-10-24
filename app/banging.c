#include <Bitbang.h>
#include <Button.h>

#define BUFFER_SIZE 32
struct lr_cell     cells[BUFFER_SIZE] = {0};
struct linked_ring buffer             = {cells, BUFFER_SIZE};

Clock(clk, &hw.timer, TIMESTAMP);

/* Bitbang SPI output */
pin_t         clk_pin     = hw_pin(D, 2);
pin_t         mosi_pin    = hw_pin(D, 3);
pin_t         miso_pin    = hw_pin(D, 4);
pin_t         debug_pin   = hw_pin(D, 5);
pin_t *       spi_pins[]  = {&mosi_pin, &miso_pin, &debug_pin, NULL};
enum pin_mode spi_modes[] = {PIN_MODE_OUTPUT, PIN_MODE_INPUT, PIN_MODE_OUTPUT};
Bitbang(spi, _({.io        = &hw.io,
                .clock     = &clk.state.time,
                .baudrate  = 9600,
                .msb_first = true,
                .pins      = (void **)spi_pins,
                .clk_pin   = &clk_pin,
                .modes     = spi_modes,
                .buffer    = &buffer}));


/* Push button that increment counter on release */
pin_t        counter_pin   = hw_pin(D, 0);
unsigned int counter_index = 0;
unsigned int debug_index   = 255;
void         send_number(Component *trigger)
{
    counter_index++;
    debug_index--;

    lr_write(&buffer, counter_index, &mosi_pin);
    lr_write(&buffer, debug_index, &debug_pin);
}

Button(counter, _({
                    .io  = &hw.io,
                    .pin = &counter_pin,

                    .clock = &clk.state.time,

                    .type            = BUTTON_PUSH_PULLUP,
                    .bounce_delay_ms = 100,

                    .onRelease = send_number,
                }));


/* Indication */
pin_t led_pin = hw_pin(B, 0);
IO_new(led, _({
                .io   = &hw.io,
                .pin  = &led_pin,
                .mode = IO_OUTPUT,
            }));


int main(void)
{
    send_number(0);

    loop(clk, spi, counter)
    {
        apply(IO, led,
              _({
                  .level = counter_index % 5 == 0,
              }));
    }
}
