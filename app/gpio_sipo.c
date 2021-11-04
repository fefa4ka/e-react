#include <Button.h>
#include <SIPO.h>
#include <SPIComputer.h>

#define BAUDRATE    9600
#define BUFFER_SIZE 16
struct lr_cell     cells[BUFFER_SIZE] = {0};
struct linked_ring buffer             = {cells, BUFFER_SIZE};


pin_t clk_pin   = hw_pin(CLK, 0);
pin_t data_pin  = hw_pin(DATA, 0);
pin_t reset_pin = hw_pin(RESET, 0);
pin_t btn_pin   = hw_pin(BTN, 0);


Clock(clk, &hw.timer, TIMESTAMP);

SIPO(gpio_array, _({.io       = &hw.io,
                    .clock    = &clk.state.time,
                    .baudrate = 1200,
                    .buffer   = &buffer,
                    .bus      = {
                        .data_pin  = &data_pin,
                        .reset_pin = &reset_pin,
                        .clk_pin   = &clk_pin,
                    }}));

struct SIPO_pin spi_clk_pin     = {&gpio_array, 2};
struct SIPO_pin cipo_pin        = {&gpio_array, 0};
struct SIPO_pin copi_pin        = {&gpio_array, 3};
struct SIPO_pin chip_select_pin = {&gpio_array, 4};

struct SIPO_pin led_pin = {&gpio_array, 1};

SPIComputer(spi, _({.io       = &SIPO_handler,
                    .clock    = &clk.state.time,
                    .baudrate = 1200,
                    .buffer   = &buffer,
                    .bus      = {
                        .copi_pin = &copi_pin,
                        .cipo_pin = &cipo_pin,
                        .clk_pin  = &spi_clk_pin,
                    }}));

/* Application handlers */
uint8_t counter;
void    send_number(Component *trigger)
{
    counter++;

    SPI_write(&spi, 1, 2, &chip_select_pin);
}
Button(pusher, _({.io              = &hw.io,
                  .pin             = &btn_pin,
                  .type            = BUTTON_PUSH_PULLUP,
                  .clock           = &clk.state.time,
                  .bounce_delay_ms = 100,

                  .onRelease = send_number}));

int main(void)
{
    log_init();
    send_number(NULL);
    loop(clk, gpio_array, spi, pusher)
    {

        /*
         react (IO, led,
                _ ({ .io    = &SIPO_handler,
                     .pin   = &state.led_pin,
                     .mode  = IO_OUTPUT,
                     .level = Button_isPressed (&pusher) }));
                     */
    }

    return 0;
}
