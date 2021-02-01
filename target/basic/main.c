#include <IO.h>

pin_t debug_pin = hw_pin(D, 4);

int
main (void)
{
    bool debug = false;
    // Define React components
    IO (pin);

    // Event-loop
    while (true) {
        // Debug Step
        debug = !debug;

        react (IO, pin,
               _ ({
                   .io    = &hw.io,
                   .pin   = &debug_pin,
                   .mode  = IO_OUTPUT,
                   .level = debug,
               }));

    }
}
