#ifndef block_UART_h
#define block_UART_h

#include "../../react/component.h"
#include "../../hal/api.h"
#include "../../utils/circular.h"

typedef struct {
    uart_handler        *uart;
    unsigned int        baudrate;

    enum eCommunicationMode     mode; 
    struct sCircularBuffer     *buffer;
    

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
