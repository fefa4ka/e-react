#include <IO.h>

#ifdef ARCH_STM8L
    #define DEBUG_PIN hw_pin(D, 6)
#else
    #define DEBUG_PIN hw_pin(B, 0)
#endif

pin_t debug_pin = DEBUG_PIN;
IO (pin);

int
main (void)
{
    bool debug = false;

    // Event-loop
    while(true) {
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
