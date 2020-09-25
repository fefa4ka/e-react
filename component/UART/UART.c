#include "UART.h"

willMount(UART) {
    props->uart->init(props->baudrate);
}

shouldUpdate(UART) {
    unsigned char sending;

    if(props->uart->isDataReceived()) {
        state->mode = eCommunicationModeReceiver;
        return true;
    }

    if(props->uart->isTransmitReady()) {
        state->mode = eCommunicationModeTransceiver;

        if(rb_read(props->tx_buffer, &sending) == eErrorBufferEmpty) {
            return false;
        } else {
            state->sending = sending;

            if(sending == 0) {
                rb_read(props->tx_buffer, &sending);
                state->mode = eCommunicationModeReceiver;
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
    if(state->mode == eCommunicationModeTransceiver) {
        props->uart->transmit(state->sending);

        if(props->onTransmit) props->onTransmit(self);

    } else if(state->mode == eCommunicationModeReceiver) {
        state->sending = props->uart->receive();
        rb_write(props->rx_buffer, state->sending); 

        if(props->onReceive) props->onReceive(self);
    }
}

didMount(UART) { }
didUnmount(UART) { }

didUpdate(UART) {
    if(state->sending == '\r') {
        if(state->mode == eCommunicationModeReceiver) {
            if(props->onReceiveLine) props->onReceiveLine(self);
        } else {
            if(props->onTransmitLine) props->onTransmitLine(self);
        }
    }

    state->sending = 0;
}


React_Constructor(UART);
