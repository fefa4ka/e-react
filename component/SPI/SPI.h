#pragma once

#include <Bitbang.h>
#include <component.h>
#include <stdbool.h>

struct SPI_buffer
{
    struct ring_buffer output;
    pin_t **           chip_select;
};

typedef struct
{
    Bitbang_blockProps bitbang;

    io_handler *         io;
    struct rtc_datetime *time;

    unsigned int       baudrate;
    struct SPI_buffer *buffer;

    void *miso_pin;
    void *mosi_pin;
    void *clk_pin;
} SPI_blockProps;

typedef struct
{
    Component           bitbanger;
    Bitbang_blockState  bitbang_state;
    Bitbang_blockProps  bitbang_props;
    void *              pins[3];
    struct ring_buffer *buffers[2];

    unsigned char      input_buffer_data;
    struct ring_buffer input_buffer;
    struct callback    SPI_init;
    struct callback    SPI_receive;
} SPI_blockState;

React_Header (SPI);
#define SPI(instance) component (SPI, instance)

void SPI_write (Component *instance, unsigned char address,
                unsigned char value, pin_t *chip_select_pin);

void SPI_read (Component *instance, unsigned char address,
               struct callback *callback, pin_t *chip_select_pin);
