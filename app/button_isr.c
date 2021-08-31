#include <Button.h>
#include <IO.h>
#include <Scheduler.h>

/* Datetime couting */
Clock(timer, &hw.timer, TIMESTAMP);
Scheduler(scheduler, 3, _({.timer = &hw.timer}));
timer_handler *scheduler_timer_handler = Scheduler_handler(scheduler);

/* Indicator */
IO(led);
pin_t led_pin = hw_pin(B, 1);

/* Push button for led blinking */
pin_t push_pin = hw_pin(D, 1);
Button(pusher, _({
                   .io  = &hw.io,
                   .pin = &push_pin,

                   .clock = &timer.state.time,
                   .timer = &scheduler_timer_handler,

                   .type            = BTN_PUSH_PULLUP,
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
        unmount(pusher);
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

                     .clock = &timer.state.time,
                     .timer = scheduler_timer_handler,

                     .type            = BTN_TOGGLE_PULLUP,
                     .bounce_delay_ms = 1000,

                     .onToggle = switcher_toggle,
                 }));


int main(void)
{
    /* Interrupt implementation */
    use(switcher);

    loop(timer, scheduler);

    return 0;
}
