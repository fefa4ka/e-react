#include <Bitbang.h>
#include <Button.h>
#include <Timer.h>

#define BAUDRATE    9600
#define BUFFER_SIZE 8


/* React components */
Timer(timer, &hw.timer, TIMESTAMP);


/* SPI on top of Bitbang */
unsigned char    output_buffer[BUFFER_SIZE];
unsigned char    input_buffer[BUFFER_SIZE];
struct callback *callback_buffer[BUFFER_SIZE]    = {0};
pin_t *          chip_select_buffer[BUFFER_SIZE] = {0};
struct {
    struct ring_buffer input_buffer;
    struct ring_buffer output_buffer;


    pin_t clk_pin;

    pin_t miso_pin;
    pin_t mosi_pin;
} spi_state = {
    .clk_pin       = hw_pin(D, 1),
    .mosi_pin      = hw_pin(D, 2),
    .miso_pin      = hw_pin(D, 3),
    .input_buffer  = {input_buffer, BUFFER_SIZE},
    .output_buffer = {output_buffer, BUFFER_SIZE},
};

/**
 * \brief    Write data to address
 */
void write_address(unsigned char address, unsigned char value,
                   pin_t *chip_select_pin)
{
    rb_write(&spi_state.output_buffer, address);
    rb_write(&spi_state.output_buffer, value);
    chip_select_buffer[spi_state.output_buffer.read] = chip_select_pin;
}

/**
 * \brief    Fire callback after data will be written to SPI bus 
 */
void send_command_callback(unsigned char address, unsigned char value,
                           pin_t *chip_select_pin, struct callback *callback)
{
    write_address(address, value, chip_select_pin);
    callback_buffer[spi_state.output_buffer.read] = callback;
}

/**
 * \brief    Read data by address
 */
void read_address(unsigned char address, pin_t *chip_select_pin,
                  struct callback *callback)
{
    send_command_callback(address, 0, chip_select_pin, callback);
}

/*
 * \brief    Select chip before data transmission 
 */
void spi_start(Component *instance)
{
    pin_t *chip_select_pin = chip_select_buffer[spi_state.output_buffer.read];
    if (chip_select_pin) {
        hw.io.out(chip_select_pin);
        hw.io.on(chip_select_pin);
    }
}

void spi_receive(Component *instance)
{
    pin_t *chip_select_pin    = chip_select_buffer[spi_state.output_buffer.read];
    struct callback *callback = callback_buffer[spi_state.output_buffer.read];
    unsigned char    data;
    rb_read(&spi_state.input_buffer, &data);

    if (callback) {
        callback->method(data, callback->argument);
        callback_buffer[spi_state.output_buffer.read] = NULL;
    }
    if (chip_select_pin) {
        hw.io.off(chip_select_pin);
        chip_select_buffer[spi_state.output_buffer.read] = NULL;
    }
}

pin_t *             spi_pins[]    = {&spi_state.mosi_pin, &spi_state.miso_pin, NULL};
struct ring_buffer *spi_buffers[] = {&spi_state.output_buffer, &spi_state.input_buffer};
enum pin_mode       spi_modes[]   = {PIN_MODE_OUTPUT, PIN_MODE_INPUT};

Bitbang(spi, _({
                 .io       = &hw.io,
                 .timer    = &timer.state.time,

                 .baudrate = BAUDRATE,

                 .pins     = spi_pins,
                 .clock    = &spi_state.clk_pin,
                 .modes    = spi_modes,
                 .buffers  = spi_buffers,

                 .onStart       = spi_start,
                 .onTransmitted = spi_receive,
             }));


/* Counter */
unsigned char counter_index = 0;
void counter_increment(Component *trigger)
{
    counter_index++;

    /* To SPI output */
    rb_write(&spi_state.output_buffer, counter_index);
}

pin_t counter_pin = hw_pin(B, 2);
Button(counter, _({
                    .io              = &hw.io,
                    .pin             = &counter_pin,

                    .timer           = &timer.state.time,

                    .type            = BUTTON_PUSH_PULLUP,
                    .bounce_delay_ms = 100,

                    .onRelease = counter_increment,
                }));



int main(void)
{
    /* Welcome count */
    counter_increment(NULL);

    loop(timer, counter, spi) {}

    return 0;
}
