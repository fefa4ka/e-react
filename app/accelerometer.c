
#include <Button.h>
#include <Calendar.h>
#include <MMA7455.h>
#include <SPI.h>
#include <UART.h>

struct device
{
    struct rtc_datetime time;

    struct
    {
        struct ring_buffer output_buffer;
        struct ring_buffer input_buffer;
    } uart;

    struct SPI_buffer spi_bus_buffer;

    pin_t clk_pin;

    pin_t miso_pin;
    pin_t mosi_pin;

    pin_t mma7455_pin;
};


#define VERSION 1
#define UART_BAUDRATE 9600
#define SPI_BAUDRATE  9600
// Buffers allication
#define BUFFER_SIZE             128
#define SPI_BUFFER_SIZE         BUFFER_SIZE
#define UART_BUFFER_SIZE        BUFFER_SIZE
#define UART_OUTPUT_BUFFER_SIZE UART_BUFFER_SIZE
#define UART_INPUT_BUFFER_SIZE  UART_BUFFER_SIZE

unsigned char uart_output_buffer[UART_OUTPUT_BUFFER_SIZE];
unsigned char uart_input_buffer[UART_INPUT_BUFFER_SIZE];

unsigned char   spi_output_buffer[SPI_BUFFER_SIZE];
struct callback spi_callback_buffer[SPI_BUFFER_SIZE]    = { 0 };
pin_t *         spi_chip_select_buffer[SPI_BUFFER_SIZE] = { 0 };

struct device state = {
    .time = { 0 },
    .uart = { .output_buffer = { uart_output_buffer, UART_BUFFER_SIZE },
              .input_buffer  = { uart_input_buffer, UART_BUFFER_SIZE } },
    .spi_bus_buffer = { .output      = { spi_output_buffer, BUFFER_SIZE },
                        .callback    = spi_callback_buffer,
                        .chip_select = spi_chip_select_buffer },
    .clk_pin        = hw_pin (B, 2),
    .mosi_pin       = hw_pin (B, 4),
    .miso_pin       = hw_pin (B, 5),
    .mma7455_pin    = hw_pin (B, 1),
};

void
log_string (char *message)
{
    rb_write_string (&state.uart.output_buffer, message);
}

void
log_num (char *message, int number)
{
    rb_write_string (&state.uart.output_buffer, message);
    rb_write_string (&state.uart.output_buffer, itoa (number));
}

void
log_accelerometer (Component *trigger)
{
    MMA7455_state_t *state = (MMA7455_state_t *)trigger->state;
    log_num ("x: ", state->force.value.x);
    log_num (" y: ", state->force.value.y);
    log_num (" z: ", state->force.value.z);
    log_string ("\r\n");
}


void print_version(Component *trigger) 
{
    log_num("\r\ne-react ver. ", VERSION);
    log_num(".", BUILD_NUM);
    log_string("\r\n"); 
}


int
main (void)
{
    Time (datetime);
    UART (serial);
    SPI (spi_bus);
    MMA7455 (sensor);

    print_version(NULL);

    while (true) {
        react (Time, datetime,
               _ ({
                   .timer = &hw.timer,
                   .time  = &state.time,
                   .onSecond = print_version
               }));

        react (UART, serial,
               _ ({
                   .uart      = &hw.uart,
                   .baudrate  = UART_BAUDRATE,
                   .tx_buffer = &state.uart.output_buffer,
                   .rx_buffer = &state.uart.input_buffer,
               }));

        react (SPI, spi_bus,
               _ ({ .io       = &hw.io,
                    .baudrate = SPI_BAUDRATE,
                    .buffer   = &state.spi_bus_buffer,
                    .miso_pin = &state.miso_pin,
                    .mosi_pin = &state.mosi_pin,
                    .clk_pin  = &state.clk_pin,
                    .time     = &state.time }));

        react (MMA7455, sensor,
               _ ({
                   .spi             = &spi_bus,
                   .chip_select_pin = &state.mma7455_pin,
                   .onChange        = log_accelerometer,
               }));
    }
}
