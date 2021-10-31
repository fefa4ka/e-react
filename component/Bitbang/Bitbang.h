#pragma once

#include <Clock.h>
#include <linked_ring.h>
#include <stddef.h>

#define Bitbang(instance, props) define(Bitbang, instance, _(props), 0)
#define Bitbang_new(instance, state)                                           \
    React_Define_WithState(Bitbang, instance, state)


#ifndef BITBANG_MAX_LINES
    #define BITBANG_MAX_LINES 8
#endif

typedef struct {
    io_handler *  io;
    struct Clock *clock;

    // TODO: use macros and pack to uint8_t
    uint16_t baudrate;

    enum io_level chip_select_polarity : 1;
    enum bit_order bit_order : 1; /* Order in which sending and reading bits */

    void **        pins;        /* List of pins used for lines */
    enum pin_mode *modes; /* Related pins mapping for IO mode configuration */
    struct linked_ring *buffer;  /* Bitbang data buffers for each line */
    void *              clk_pin; /* Optional clock pin */

    struct callback *onStart;
    struct callback *onTransmitted;
} Bitbang_props_t;

typedef struct {
    bool operating : 1;
    bool clock : 1;

    uint8_t data[BITBANG_MAX_LINES]; /* Current sending and reading bytes
                                              for each data line */

    int8_t position;     /* Data bit index now operating */

    uint16_t tick;
} Bitbang_state_t;

React_Header(Bitbang);
