#include "Serial.h"

willMount(Serial) {
    props->handler->init(&props->baudrate);
}

shouldUpdate(Serial) {
    unsigned char sending;

    if(props->handler->isDataReceived()) {
        state->mode = COMMUNICATION_MODE_RECEIVER;
        return true;
    }

    if(props->handler->isTransmitReady()) {
        // Send Handler for next received signal
        state->mode = COMMUNICATION_MODE_TRANSCIEVER;

        if(rb_read(state->tx_buffer, &sending) == ERROR_BUFFER_EMPTY) {
            return false;
        } else {
            state->sending = sending;

            if(sending == 0) {
                rb_read(state->tx_buffer, &sending);
                state->mode = COMMUNICATION_MODE_RECEIVER;
            } else {
                return true;
            }

            return true;
        }
    }

    return false;
}

willUpdate(Serial) { }

release(Serial) {
    if(state->mode == COMMUNICATION_MODE_TRANSCIEVER) {
        props->handler->transmit(state->sending);

        if(props->onTransmit) props->onTransmit(self);

    } else if(state->mode == COMMUNICATION_MODE_RECEIVER) {
        state->sending = props->handler->receive();
        rb_write(state->rx_buffer, state->sending);

        if(props->onReceive) props->onReceive(self);
    }
}

didMount(Serial) { }

didUpdate(Serial) {
    if(state->sending == '\r') {
        if(state->mode == COMMUNICATION_MODE_RECEIVER) {
            if(props->onReceiveLine) props->onReceiveLine(self);
        } else {
            if(props->onTransmitLine) props->onTransmitLine(self);
        }
    }

    state->sending = 0;
}

