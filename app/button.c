#include <Button.h>
#include <IO.h>
#include <Timer.h>

/* Datetime couting */
Timer(timer, &hw.timer, 0);

/* Indicator */
IO(led);
pin_t led_pin = hw_pin(B, 1);

/* Mode switcher. Led could blink When enabled. */
bool  enabled      = true;
pin_t switcher_pin = hw_pin(D, 0);
void  switcher_toggle(Component *trigger) { enabled = !enabled; }
Button(switcher,
       _({
           .io  = &hw.io,
           .pin = &switcher_pin,

           .timer = &timer.state.time,

           .type            = BTN_TOGGLE_PULLUP,
           .bounce_delay_ms = 1000,

           .onToggle = switcher_toggle,
       }));

/* Push button for led blinking */
pin_t push_pin = hw_pin(D, 1);
Button(pusher,
       _({
           .io  = &hw.io,
           .pin = &push_pin,

           .timer = &timer.state.time,

           .type            = BTN_PUSH_PULLUP,
           .bounce_delay_ms = 100,
       }));


int main(void)
{
    // Event-loop
    loop(timer, switcher)
    {
        if (enabled) {
            use(pusher);

            apply(IO, led,
                  _({.io    = &hw.io,
                     .pin   = &led_pin,
                     .mode  = IO_OUTPUT,
                     .level = Button_isPressed(&pusher) }));
        }
    }

    return 0;
}
