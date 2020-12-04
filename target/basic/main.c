#include <IO.h>

struct device
{
    bool  debug;
    pin_t pin;
};


struct device state = {
    .debug = true,
    .pin = hw_pin(D, 4)
};


int
main (void)
{
    // Define React components
    IO (pin);

    // Event-loop
    while (true) {
        // Debug Step
        state.debug = !state.debug;

        react (IO, pin,
               _ ({
                   .io    = &(hw.io),
                   .pin   = &state.pin,
                   .mode  = IO_OUTPUT,
                   .level = state.debug,
               }));
    }
}
