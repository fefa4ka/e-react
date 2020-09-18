#include "Button.h"



willMount(Button) {
    /* Setup pin as input */
    props->io->in(props->pin);
}

shouldUpdate(Button) {
    bool level = props->io->get(props->pin); 

    unsigned int passed = props->time->time_ms - state->tick;
    

    // Second check after bounce_delay_ms
    if(state->tick && passed >= props->bounce_delay_ms) {
        return true;
    }

    /* Already pressed */
    if(state->level == 1 && level == 1 && state->pressed) {
        return false;
    }

    /* First high level detected */
    if(state->level == 0 && level != 0 && state->tick == 0) {
        return true;
    }
    

    state->level = level;

    // Another checks after pressed
    if(props->type == push && state->pressed && level == 0) {
        // Push button unpressed after release
        return true;
    }

    return false;
}

willUpdate(Button) {
    // Actual state
    state->level = props->io->get(props->pin); 

    // Set initial tick to start count delay
    if(!state->tick && state->level) {
        state->tick = props->time->time_ms;
    }
}

release(Button) {
    unsigned int passed = props->time->time_ms - state->tick;
    bool pressed = props->type == toggle 
                ? state->pressed 
                : false;

    if(state->tick && passed > props->bounce_delay_ms) {
        if(state->level) {
            pressed = props->type == toggle 
                ? !state->pressed 
                : true;
        } 

        state->tick = 0;
    }

    if(state->pressed != pressed) {
        if(props->onToggle) props->onToggle(self);

        if(pressed) {
            if(props->onPress) props->onPress(self);
        } else {
            if(props->onRelease) props->onRelease(self);
        }
    }

    state->pressed = pressed;
}

didMount(Button) { }

didUnmount(Button) { }

didUpdate(Button) { }

React_Constructor(Button);
