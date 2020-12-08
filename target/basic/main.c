#include <IO.h>

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
                   .pin   = hw_pin (D, 4),
                   .mode  = IO_OUTPUT,
                   .level = debug,
               }));

    }
}
