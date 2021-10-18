#include <IO.h>
#include <sched.h>

#ifdef ARCH_STM8L
    #define DEBUG_PIN hw_pin(D, 6)
#else
    #define DEBUG_PIN hw_pin(B, 0)
#endif
#define SENSOR_PIN hw_pin(D, 1)

pin_t debug_pin = DEBUG_PIN;
IO_new(pin, _({
                .io   = &hw.io,
                .pin  = &debug_pin,
                .mode = IO_OUTPUT,
            }));


pin_t sensor_pin = SENSOR_PIN;
IO_new(sensor, _({
                .io   = &hw.io,
                .pin  = &sensor_pin,
                .mode = IO_INPUT,
            }));

int main(void)
{
    bool debug = false;

    // Event-loop
    loop (sensor) {
        // Debug Step
        debug = !debug;

        apply(IO, pin,
              _({
                  .level = debug,
              }));
    }
}
