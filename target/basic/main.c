#include <IO.h>

struct device
{
    bool             debug;
    pin_t            pin; 
};


struct device state = {
    .debug = true,
    .pin = hw_pin(B, 1)
};


int main(void) {
    // Define React components
    IO(pin);

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

