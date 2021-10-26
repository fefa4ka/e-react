#pragma once

#include <Bitbang.h>
#include <Clock.h>

#include <linked_ring.h>

#define SPIComputer(instance, props)                                           \
    define(SPIComputer, instance, _(props), {0})

typedef struct {
    io_handler *  io;
    struct Clock *clock;

    unsigned int baudrate;

    struct linked_ring *buffer;

    struct {
        void *copi_pin;
        void *cipo_pin;
        void *spacer;
        void *clk_pin;
    } bus;
} SPIComputer_props_t;

typedef struct {
    Bitbang_Component bitbanger;

    struct callback *callback;
    void *           chip_select_pin;

    struct callback onStart;
    struct callback onReceive;
} SPIComputer_state_t;

React_Header(SPIComputer);

void SPI_write(Component *spi_ptr, unsigned char address, unsigned char value,
               void *chip_select_pin);

void SPI_read(Component *spi_ptr, unsigned char address,
              struct callback *callback, void *chip_select_pin);
