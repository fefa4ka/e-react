#pragma once

#include <Clock.h>
#include <circular.h>
#include <stddef.h>

#define Bitbang(instance, props) define(Bitbang, instance, _(props), {0})
#define Bitbang_new(instance, state)                                           \
    React_Define_WithState(Bitbang, instance, state)


#define BITBANG_MAX_LINES 8

typedef struct {
    io_handler *  io;
    struct Clock *clock;

    unsigned int baudrate;

    bool msb_first; /* Order in which sending and reading bits */

    unsigned short pins_number; /* Number of data lines */
    void **        pins;        /* List of pins used for lines */
    enum pin_mode *modes; /* Related pins mapping for IO mode configuration */
    struct ring_buffer **buffers; /* Bitbang data buffers for each line */
    void *               clk_pin; /* Optional clock pin */

    struct callback *onStart;
    struct callback *onTransmitted;
} Bitbang_props_t;

typedef struct {
    bool sending;

    unsigned char data[BITBANG_MAX_LINES]; /* Current sending and reading bytes
                                              for each data line */
    short position;                        /* Data bit index now operating */

    unsigned long tick;
} Bitbang_state_t;

React_Header(Bitbang);
