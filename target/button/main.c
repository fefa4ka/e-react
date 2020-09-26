#include <Time.h>
#include <Button.h>

struct device
{
    struct rtc_datetime    time;
    bool             led_enabled;

    pin_t            switcher_pin; 
    pin_t            led_pin; 
};


struct device state = {
    .time = {0},
    .led_enabled = true,
    .led_pin = hw_pin(B, 1),
    .switcher_pin = hw_pin(B, 2)
};


void led_toggle(Component *trigger) {
    state.led_enabled = !state.led_enabled;
}

int main(void) {
    // Define React components
    component(Time, datetime);
    component(IO, led);
    component(Button, switcher);

    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(hw.timer),
            .time = &state.time,
        } to (datetime);

        react (Button) {
            .io = &(hw.io),
            .pin = &state.switcher_pin,
            .type = toggle,
            .time = &state.time,
            .bounce_delay_ms = 100,
            .onToggle = led_toggle
        } to (switcher);

        React (IO) {
            .io = &(hw.io),
            .pin = &state.led_pin,
            .mode = output,
            .level = state.led_enabled
                ? high
                : low
        } to (led);
    }

    return 0;
}


