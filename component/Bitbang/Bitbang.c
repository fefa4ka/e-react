#include "Bitbang.h"

#define bit_value(data, bit) ((data >> bit) & 1)      /** Return Data.Y value   **/
#define bit_set(data, bit)    data |= (1 << bit)    /** Set Data.Y   to 1    **/
#define bit_clear(data, bit)  data &= ~(1 << bit)   /** Clear Data.Y to 0    **/

willMount(Bitbang) {
    // setup pins
    void *pin = props->pins;
    pin_mode_t *mode = props->modes; 

    while(pin) {
        if(*mode == input_e) {
            props->io->in(pin);
        } else {
            props->io->out(pin);
        }

        pin++;
        mode++;
    }
}

shouldUpdate(Bitbang) {
    // Component free for operation
    if(*state->data == 0) {
        if(rb_length(props->buffers)) {
            /* Data available */
            return true;
        }

        /* No data available */
        return false;
    }

    // Change state on baudrate
    unsigned long bit_duration = 1000000 / props->baudrate;
    if(props->time->time_us + props->time->step_us - state->tick >= bit_duration) {
        return true;
    }
    
    return false;
}

willUpdate(Bitbang) {
    unsigned char        *data   = state->data;
    void                 *pin    = props->pins;
    pin_mode_t           *mode   = props->modes;
    struct ring_buffer_s *buffer = props->buffers;
    bool                 sending = state->sending;

    state->tick = props->time->time_us;

    while(pin) {
        if(*mode == output_e) {
            if(state->position == -1) {
                /* Write full byte from input */
                rb_write(buffer, *data);
            } else {
                /* Read bit */
                if(props->io->get(pin)) {
                    bit_set(*data, state->position);
                } else {
                    bit_clear(*data, state->position);
                }
            }
        } else { 
            /* Read new byte for output */
            if(state->sending == false) {
                rb_read(buffer, data);
                sending = true;
            }
        }

        pin++;
        data++;
        mode++;
        buffer++;
    }
 
    state->sending = sending;
    if(props->clock) props->io->off(props->clock);
}

release(Bitbang) {
    void            *pin  = props->pins;
    unsigned char   *data = state->data;
    pin_mode_t      *mode = props->modes;

    while(pin) {
        if(*mode == input_e) {
            if(bit_value(*data, state->position)) {
                props->io->on(pin);
            } else {
                props->io->off(pin);
            }
        } 
        data++;
        pin++;
        mode++;
    }
}

didMount(Bitbang) { }
didUnmount(Bitbang) { }


didUpdate(Bitbang) {
    if(props->clock) props->io->on(props->clock);
    
    /* Increment bit position */
    if(state->sending) {
        if(state->position == 7) {
            /* clear session */
            state->sending = false;
            state->position = -1;
        } else {
            state->position++;
        }
    }
}


React_Constructor(Bitbang);
