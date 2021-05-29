#pragma once

#include <circular.h>
#include <component.h>

#define Serial(instance, buffer_size, props)                                   \
    unsigned char      instance##_tx_buffer_items[buffer_size];                \
    unsigned char      instance##_rx_buffer_items[buffer_size];                \
    struct ring_buffer instance##_tx_buffer                                    \
        = {instance##_tx_buffer_items, buffer_size};                           \
    struct ring_buffer instance##_rx_buffer                                    \
        = {instance##_rx_buffer_items, buffer_size};                           \
    define(Serial, instance, _(props),                                            \
           _({                                                                 \
               .tx_buffer = &instance##_tx_buffer,                             \
               .rx_buffer = &instance##_rx_buffer,                             \
           }))


#define Serial_write(instance, message)                                        \
    rb_write(React_State(Serial, instance, tx_buffer), (unsigned char *)message)
#define Serial_write_string(instance, message)                                 \
    rb_write_string(React_State(Serial, instance, tx_buffer), (unsigned char *)message)
#define Serial_read(instance, data)                                            \
    rb_read(React_State(Serial, instance, rx_buffer), data)

typedef struct {
    serial_handler *handler;
    unsigned int    baudrate;

    void (*onReceive)(Component *instance);
    void (*onReceiveLine)(Component *instance);
    void (*onTransmit)(Component *instance);
    void (*onTransmitLine)(Component *instance);
} Serial_props_t;

typedef struct {
    struct ring_buffer *tx_buffer;
    struct ring_buffer *rx_buffer;

    enum communication_mode mode;
    unsigned char           sending; /* Current sending or receiving byte */
} Serial_state_t;

React_Header(Serial);
