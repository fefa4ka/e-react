#ifndef block_UART_h
#define block_UART_h

#include <component.h>
#include <api.h>
#include <circular.h>

typedef struct {
    uart_handler        *uart;
    unsigned int        baudrate;

    enum eCommunicationMode     mode; 
    struct ring_buffer_s        *buffer;
    

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

#endif
