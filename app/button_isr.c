#include <Button.h>
#include <IO.h>
#include <Clock.h>
#include <ISR.h>

enum ISR_system(GPIO, SPI, TIMER);

/* Datetime couting */
Clock(clock, &hw.timer, TIMESTAMP);

/* Indicator */
IO(led);
pin_t led_pin = hw_pin(B, 1);

/* Push button for led blinking */
pin_t push_pin = hw_pin(D, 1);
Button(pusher, _({
                   .io  = &hw.io,
                   .pin = &push_pin,

                   .clock = &clock.state.time,
                   .timer = &hw.timer,

                   .type            = BUTTON_PUSH_PULLUP,
                   .bounce_delay_ms = 100,
               }));

/* Mode switcher. Led could blink When enabled. */
bool  enabled      = true;
pin_t switcher_pin = hw_pin(D, 0);
void  switcher_toggle(Component *trigger)
{
    enabled = !enabled;

    if (enabled) {
        use(pusher);
    } else {
        shut(pusher);
    }

    react(IO, led,
          _({.io    = &hw.io,
             .pin   = &led_pin,
             .mode  = IO_OUTPUT,
             .level = Button_isPressed(&pusher)}));
}
Button(switcher, _({
                     .io  = &hw.io,
                     .pin = &switcher_pin,

                     .clock = &clock.state.time,
                     .timer = &hw.timer,

                     .type            = BUTTON_TOGGLE_PULLUP,
                     .bounce_delay_ms = 1000,

                     .onToggle = switcher_toggle,
                 }));


int main(void)
{
    /* Interrupt implementation */
    use(switcher);

    loop(clock);

    return 0;
}
