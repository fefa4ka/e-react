#include "UART.h"

willMount(UART_block) {
    props->uart->init(&props->baudrate);
}

shouldUpdate(UART_block) {
    unsigned char sending;
    if(props->mode == eCommunicationModeTransceiver 
            && props->uart->isTransmitReady()) {
        state->mode = eCommunicationModeTransceiver;

        if(CBRead(props->buffer, &sending) == eErrorBufferEmpty) {
            return false;
        } else {
            state->sending = sending;

            if(sending == NULL) {
                CBRead(props->buffer, &sending);
                state->mode = eCommunicationModeReceiver;
            } else {
                return true;
            }

            return true;
        }
    }

    if((props->mode == eCommunicationModeReceiver|| state->mode == eCommunicationModeReceiver)
            && props->uart->isDataReceived()) {
        state->mode = eCommunicationModeReceiver;
        return true;
    }

    return false;
}

willUpdate(UART_block) { }

release(UART_block) {
    if(state->mode == eCommunicationModeTransceiver) {
        props->uart->transmit(state->sending);

        if(props->onTransmit) props->onTransmit(self);

    } else if(state->mode == eCommunicationModeReceiver) {
        state->sending = props->uart->receive();
        CBWrite(props->buffer, state->sending); 

        if(props->onReceive) props->onReceive(self);
    }
}

didMount(UART_block) { }
didUnmount(UART_block) { }

didUpdate(UART_block) {
    if(state->sending == '\n') {
        if(props->mode == eCommunicationModeReceiver) {
            if(props->onReceiveLine) props->onReceiveLine(self);
        } else {
            if(props->onTransmitLine) props->onTransmitLine(self);
        }
    }

    state->sending = 0;
}


React_Constructor(UART_block);
