#pragma once

#include <Bitbang.h>

#define SIPO(instance, props) define(SIPO, instance, _(props), 0)

struct SIPO_pin {
    Component *bus;
    uint8_t    number;
};

typedef struct {
    io_handler *  io;
    struct Clock *clock;

    uint16_t baudrate;

    struct linked_ring *buffer;

    struct {
        void *data_pin;
        void *spacer;
        void *reset_pin;
        void *clk_pin;
    } bus;
} SIPO_props_t;

typedef struct {
    Bitbang_Component bitbanger;

    bool            reseting : 1;
    uint8_t         pins_state;
    uint8_t         released_state;
    struct callback SIPO_reset;
    struct callback SIPO_next;
} SIPO_state_t;

extern io_handler SIPO_handler;

React_Header(SIPO);
