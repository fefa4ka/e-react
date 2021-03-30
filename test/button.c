#include "unit.h"

#include <IO.h>
#include <Timer.h>
#include <Button.h>


/* Datetime couting */
Timer(timer, &hw.timer, 0);

/* Indicator */
IO(led);
pin_t led_pin = hw_pin(B, 1);

/* Mode switcher. Led could blink When enabled. */
bool  enabled      = false;
pin_t switcher_pin = hw_pin(D, 1);
void  switcher_toggle(Component *trigger) { log_info("Switch toggled"); enabled = !enabled; }
void  switcher_pressed(Component *trigger) { log_info("Switch pressed"); }
void  switcher_released(Component *trigger) { log_info("Switch released"); }

Button(switcher,
       _({
           .io  = &hw.io,
           .pin = &switcher_pin,

           .timer = &timer.state.time,

           .type            = BTN_TOGGLE,
           .bounce_delay_ms = 1000,

           .onToggle = switcher_toggle,
           .onRelease = switcher_released,
           .onPress = switcher_pressed
       }));

/* Push button for led blinking */
pin_t pusher_pin = hw_pin(D, 0);
void  pusher_toggle(Component *trigger) { log_info("Push toggle"); }
void  pusher_pressed(Component *trigger) { log_info("Push pressed"); }
void  pusher_released(Component *trigger) { log_info("Push released"); }
Button(pusher,
       _({
           .io  = &hw.io,
           .pin = &pusher_pin,

           .timer = &timer.state.time,

           .type            = BTN_PUSH,
           .bounce_delay_ms = 100,

           .onToggle = pusher_toggle,
           .onRelease = pusher_released,
           .onPress = pusher_pressed
       }));


test(long_push) {
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
}

// Another thread time dependent run test cases
void long_push()
{
    sleep(1);
    test_assert(enabled == false, "Should disabled at first");

    hw.io.on(&switcher_pin);
    usleep(500000);
    hw.io.off(&switcher_pin);
    sleep(1);
    test_assert(enabled == false, "Should disabled after half sec");

    hw.io.on(&switcher_pin);
    usleep(1100000);
    hw.io.off(&switcher_pin);
    usleep(1000);
    test_assert(enabled == true, "Should enabled after switch for 1 sec");

    hw.io.on(&pusher_pin);
    usleep(900);
    test_assert(Button_isPressed(&pusher) == false, "Button unpushed after 10 ms");
    usleep(9000);
    test_assert(Button_isPressed(&pusher), "Button pushed after 100 ms");
    test_assert(hw.io.get(&led_pin), "Led is on");
    hw.io.off(&pusher_pin);
    usleep(50000);
    test_assert(hw.io.get(&led_pin) == false, "Led is off");
    test_assert(Button_isPressed(&pusher) == false, "Button unpushed");

    hw.io.on(&switcher_pin);
    usleep(500000);
    hw.io.off(&switcher_pin);
    usleep(1100000);
    test_assert(enabled == true, "Should enabled after half sec");

    hw.io.on(&switcher_pin);
    usleep(10000);
    hw.io.off(&switcher_pin);
    usleep(10000);
    hw.io.on(&switcher_pin);
    usleep(10000);
    hw.io.off(&switcher_pin);
    sleep(1);
    test_assert(enabled == true, "Should enabled after half sec");

    hw.io.on(&switcher_pin);
    usleep(9000);
    hw.io.off(&switcher_pin);
    usleep(1000);
    hw.io.on(&switcher_pin);
    usleep(1000000);
    hw.io.off(&switcher_pin);
    usleep(1000);
    test_assert(enabled == false, "Should disabled after switch for 1 sec");

    hw.io.on(&pusher_pin);
    usleep(900);
    test_assert(Button_isPressed(&pusher) == false, "Button unpushed after 10 ms");
    usleep(9000);
    test_assert(Button_isPressed(&pusher) == false, "Button unpushed after 100 ms");
    test_assert(hw.io.get(&led_pin) == false, "Led is off");
    hw.io.off(&pusher_pin);
    usleep(50000);
    test_assert(hw.io.get(&led_pin) == false, "Led is off");
    test_assert(Button_isPressed(&pusher) == false, "Button unpushed");
}
