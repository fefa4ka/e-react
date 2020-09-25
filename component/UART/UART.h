#pragma once 

#include <component.h>
#include <api.h>
#include <circular.h>

typedef struct {
    uart_handler        *uart;
    unsigned int        baudrate;

    struct ring_buffer_s        *tx_buffer;
    struct ring_buffer_s        *rx_buffer;
    

    void (*onReceive)(Component *instance);
    void (*onReceiveLine)(Component *instance);
    void (*onTransmit)(Component *instance);
    void (*onTransmitLine)(Component *instance);
} UART_blockProps;

typedef struct {
    enum eCommunicationMode  mode; 
    unsigned char            sending;
} UART_blockState;


React_Header(UART);

