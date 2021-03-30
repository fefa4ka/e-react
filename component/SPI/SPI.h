#pragma once

#include <Bitbang.h>
#include <component.h>
#include <stdbool.h>

#define SPI(instance) React_Define(SPI, instance)

struct SPI_buffer
{
    struct ring_buffer output;
    struct callback  * callback;
    void  **           chip_select;
};

typedef struct
{
    io_handler *         io;
    struct rtc_datetime *time;

    unsigned int       baudrate;
    struct SPI_buffer *buffer;

    void *miso_pin;
    void *mosi_pin;
    void *clk_pin;
} SPI_props_t;

typedef struct
{
    Component           bitbanger;
    Bitbang_state_t     bitbang_state;
    Bitbang_props_t     bitbang_props;
    void *              pins[3];
    struct ring_buffer *buffers[2];

    unsigned char      input_buffer_data[2];
    struct ring_buffer input_buffer;
    struct callback    SPI_init;
    struct callback    SPI_receive;
} SPI_state_t;

React_Header (SPI);

void SPI_write (Component *instance, unsigned char address,
                unsigned char value, void *chip_select_pin);

void SPI_read (Component *instance, unsigned char address,
               struct callback *callback, void *chip_select_pin);
