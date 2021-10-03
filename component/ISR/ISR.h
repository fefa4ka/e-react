#pragma once

#include <component.h>

#define ISR(instance, props)      define(ISR, instance, _(props), {0})
#define ISR_system_prefix(system) ISR_##system,
#define ISR_system_flag(system)   unsigned char is_##system##_fired : 1;
#define ISR_system(...)                                                        \
    ISR_system{EVAL(MAP(ISR_system_prefix, __VA_ARGS__)) ISR_UNDEFINED};       \
    union ISR_flag {                                                           \
        struct {                                                               \
            EVAL(MAP(ISR_system_flag, __VA_ARGS__))                           \
            unsigned char is_UNDEFINED_fired : 1;                                   \
        } states;                                                              \
        unsigned int state;                                                    \
    }

union ISR_handler_flag {
    struct {
        unsigned char is_processed : 1;
        unsigned char is_enabled : 1;
        unsigned char is_fired : 1;
    } states;
    unsigned char state;
};

struct ISR_handler {
    void *interrupt;
    void (*callback)(void *args);
    void *                 args;
    union ISR_handler_flag state;
};

typedef struct {
    struct ISR_handler *handlers;
    timer_handler *     timer;

} ISR_props_t;

typedef struct {
    void *       operation_hash;
    unsigned int fired; /* ISR_flag with system fired bits */

} ISR_state_t;

React_Header(ISR);
