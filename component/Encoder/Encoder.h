#pragma once

#include <Timer.h>
#include <component.h>

#define Encoder(instance, props) define(Encoder, instance, _(props), {0})

typedef struct {
    io_handler *io;
    pin_t *     pin_a;
    pin_t *     pin_b;

    unsigned int resolution;

    struct Timer *timer;

    void (*onTurn)(Component *instance);
    void (*onLeft)(Component *instance);
    void (*onRight)(Component *instance);
} Encoder_props_t;

typedef struct {
    int rpm;

    unsigned long tick_a; /* Timestamp when pin state changed */
    unsigned long tick_b;

    bool level_a;
    bool level_b;
} Encoder_state_t;

React_Header(Encoder);
