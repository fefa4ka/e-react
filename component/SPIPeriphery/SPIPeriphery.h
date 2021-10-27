#pragma once

#include <IO.h>
#include <SPIComputer.h>

#define SPIPeriphery(instance, props)                                          \
    define(SPIPeriphery, instance, _(props), {0})

typedef struct {
    io_handler *  io;

    struct linked_ring *buffer;

    struct {
        void *copi_pin;
        void *cipo_pin;
        void *clk_pin;
        void *chip_select_pin;
    } bus;

    void onStart(Component *instance);
    void onReceive(Component *instance);
} SPIPeriphery_props_t;

typedef struct {
    uint8_t address;
    uint8_t data;
    uint8_t sending;

    uint8_t bit_position;
    bool    clk_level;
} SPIPeriphery_state_t;

React_Header(SPIPeriphery);
