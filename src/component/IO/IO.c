#include "IO.h"

static inline void
IO_setPinMode(IO_blockProps *props) {
    if(props->mode == input) {
        props->io->in(props->pin);
    } else {
        props->io->out(props->pin);
    }
}

static inline void
IO_setPinState(IO_blockProps *props) {
    if(props->level == high) {
        props->io->on(props->pin);
    } else {
        props->io->off(props->pin);
    }
}

willMount(IO_block) {
    IO_setPinMode(props);
}

shouldUpdate(IO_block) {
    if (props->mode == input)
    {
        return true;
    }
    
    if(props->level != nextProps->level || props->mode != nextProps->mode || props->io->in != nextProps->io->in) {
        return true;
    }

    return false;
}

willUpdate(IO_block) {
    if (props->mode != nextProps->mode || props->io->in != nextProps->io->in)
    {
        IO_setPinMode(props);
    }
}

release(IO_block) {
    if (props->mode == output)
    {
        IO_setPinState(props); 
    } else {
        bool level = props->io->get(props->pin);
        if (state->level != level)
        {
            state->level = level;
            if(level != 0 && props->onHigh) {
                props->onHigh(self);
            }
            if(level == 0 && props->onLow) {
                props->onLow(self);
            }
            if(props->onChange) {
                props->onChange(self);
            }
        }
    }
}

didMount(IO_block) {
    if(props->onChange) {
        props->onChange(self);
    }
}

didUnmount(IO_block) { }
didUpdate(IO_block) { }

React_Constructor(IO_block);
