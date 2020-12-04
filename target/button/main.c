#include <Button.h>
#include <Calendar.h>

struct device
{
    struct rtc_datetime time;

    bool led_enabled;

    pin_t switcher_pin;
    pin_t led_pin;

};


struct device state = { .time         = { 0 },
                        .led_enabled  = true,
                        .led_pin      = hw_pin (B, 1),
                        .switcher_pin = hw_pin (B, 2) };


void
led_toggle (Component *trigger)
{
    state.led_enabled = !state.led_enabled;
}

int
main (void)
{
    // Define React components
    Time (datetime);
    IO (led);
    Button (switcher);

    // Event-loop
    while (true) {
        // Timer component, for event management and time counting
        react (Time, datetime,
               _ ({
                   .timer = &(hw.timer),
                   .time  = &state.time
               }));

        react (Button, switcher,
               _ ({
                   .io              = &(hw.io),
                   .pin             = &state.switcher_pin,
                   .type            = BTN_TOGGLE,
                   .time            = &state.time,
                   .bounce_delay_ms = 100,
                   .onToggle        = led_toggle
               }));

        react (IO, led,
               _ ({
                   .io    = &(hw.io),
                   .pin   = &state.led_pin,
                   .mode  = IO_OUTPUT,
                   .level = state.led_enabled
                                ? IO_HIGH
                                : IO_LOW 
               }));
    }

    return 0;
}
