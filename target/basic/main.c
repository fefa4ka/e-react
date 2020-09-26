#include <IO.h>


struct device
{
    bool             debug;
    pin_t            pin; 
};


struct devie state = {
    .debug = true,
    .pin = hw_pin(B, 1)
};


int main(void) {
    // Define React components
    component(IO, pin);

    // Event-loop
    while (true) { 
        // Debug Step
        state.debug = !state.debug;

        react (IO) {
            .io = &(hw.io),
            .pin = &state.pin,
            .mode = output,
            .level = state.debug, 
        } to (pin);
    }

    return 0;
}

