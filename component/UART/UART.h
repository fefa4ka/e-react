#pragma once 

#include <component.h>
#include <api.h>
#include <circular.h>

#define UART(instance) React_Define(UART, instance)

typedef struct {
    serial_handler        *uart;
    unsigned int        baudrate;

    struct ring_buffer        *tx_buffer;
    struct ring_buffer        *rx_buffer;
    

    void (*onReceive)(Component *instance);
    void (*onReceiveLine)(Component *instance);
    void (*onTransmit)(Component *instance);
    void (*onTransmitLine)(Component *instance);
} UART_props_t;

typedef struct {
    enum communication_mode  mode; 
    unsigned char            sending;
} UART_state_t;


React_Header(UART);
