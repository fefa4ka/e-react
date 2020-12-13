#pragma once

#include <Bitbang.h>
#include <component.h>
#include <stdbool.h>

struct SPI_buffer
{
    struct ring_buffer output;
    struct callback *  callback;
    pin_t **           chip_select;
};

typedef struct
{
    Bitbang_blockProps bitbang;

    io_handler *         io;
    struct rtc_datetime *time;

    unsigned int      baudrate;
    struct SPI_buffer buffer;

    pin_t *miso_pin;
    pin_t *mosi_pin;
    pin_t *clk_pin;

    void (*callback) (Component *instance);
} SPI_blockProps;

typedef struct
{
    Component           bitbanger;
    Bitbang_blockState  bitbang;
    pin_t *             pins[3];
    struct ring_buffer *buffers[2];

    unsigned char      input_buffer_data;
    struct ring_buffer input_buffer;
} SPI_blockState;

React_Header (SPI);
#define SPI(instance) component (SPI, instance)

void SPI_write (Component *instance, unsigned char address,
                unsigned char value, pin_t *chip_select_pin);

void SPI_read (Component *instance, unsigned char address,
               struct callback *callback, pin_t *chip_select_pin);
