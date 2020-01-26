#include "Button.h"



willMount(Button_block) {
    props->io->in(props->pin);
}

shouldUpdate(Button_block) {
    bool level = props->io->get(props->pin); 

    unsigned int passed = props->time->time_ms - state->tick;

    // First high event
    if(state->level == 0 && level != 0 && state->tick == 0) {
        return true;
    }
    
    // Second check agter bounce_delay_ms
    if(state->tick && passed >= props->bounce_delay_ms) {
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

willUpdate(Button_block) {
    // Actual state
    state->level = props->io->get(props->pin); 

    // Set initial tick to start count delay
    if(!state->tick && state->level) {
        state->tick = props->time->time_ms;
    }
}

release(Button_block) {
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

didMount(Button_block) { }

didUnmount(Button_block) { }

didUpdate(Button_block) { }

React_Constructor(Button_block);
