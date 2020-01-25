#include "UART.h"

willMount(UART_block) {
    props->uart->init(&props->baudrate);
}

shouldUpdate(UART_block) {
    if(props->mode == transmiter
            && props->uart->isTransmitReady()) {
        unsigned char sending;

        if(CBRead(props->buffer, &sending) == eErrorBufferEmpty) {
            return false;
        } else {
            
            state->sending = sending;
            return true;
        }
    }

    if(props->mode == receiver
            && props->uart->isDataReceived()) {
        return true;
    }

    return false;
}

willUpdate(UART_block) { }

release(UART_block) {
    if(props->mode == transmiter) {
        props->uart->transmit(state->sending);

        if(props->onTransmit) props->onTransmit(self);

    } else if(props->mode == receiver) {
        state->sending = props->uart->receive();
        CBWrite(props->buffer, state->sending); 

        if(props->onReceive) props->onReceive(self);
    }
}

didMount(UART_block) { }
didUnmount(UART_block) { }

didUpdate(UART_block) {
    if(state->sending == '\n') {
        if(props->mode == receiver) {
            if(props->onReceiveLine) props->onReceiveLine(self);
        } else {
            if(props->onTransmitLine) props->onTransmitLine(self);
        }
    }

    state->sending = 0;
}


React_Constructor(UART_block);
