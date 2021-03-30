#include "UART.h"

willMount(UART) {
    props->uart->init(&props->baudrate);
}

shouldUpdate(UART) {
    unsigned char sending;

    if(props->uart->isDataReceived()) {
        state->mode = COMMUNICATION_MODE_RECEIVER;
        return true;
    }

    if(props->uart->isTransmitReady()) {

        state->mode = COMMUNICATION_MODE_TRANSCIEVER;

        if(rb_read(props->tx_buffer, &sending) == ERROR_BUFFER_EMPTY) {
            return false;
        } else {

            state->sending = sending;

            if(sending == 0) {
                rb_read(props->tx_buffer, &sending);
                state->mode = COMMUNICATION_MODE_RECEIVER;
            } else {
                return true;
            }

            return true;
        }
    }

    return false;
}

willUpdate(UART) { }

release(UART) {
    if(state->mode == COMMUNICATION_MODE_TRANSCIEVER) {
        props->uart->transmit(state->sending);

        if(props->onTransmit) props->onTransmit(self);

    } else if(state->mode == COMMUNICATION_MODE_RECEIVER) {
        state->sending = props->uart->receive();
        rb_write(props->rx_buffer, state->sending); 

        if(props->onReceive) props->onReceive(self);
    }
}

didMount(UART) { }
didUnmount(UART) { }

didUpdate(UART) {
    if(state->sending == '\r') {
        if(state->mode == COMMUNICATION_MODE_RECEIVER) {
            if(props->onReceiveLine) props->onReceiveLine(self);
        } else {
            if(props->onTransmitLine) props->onTransmitLine(self);
        }
    }

    state->sending = 0;
}


React_Constructor(UART)
