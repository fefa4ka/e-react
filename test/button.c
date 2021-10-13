#include "unit.h"

#include <IO.h>
#include <Button.h>
#include <Clock.h>


/* Datetime couting */
Clock(clk, &hw.timer, 0);

/* Indicator */
IO(led);
pin_t led_pin = hw_pin(B, 1);

/* Mode switcher. Led could blink When enabled. */
bool   enabled      = false;
pin_t  switcher_pin = hw_pin(D, 1);
void   switcher_toggle(Component *trigger) { enabled = !enabled; }
void   switcher_pressed(Component *trigger) { }
void   switcher_released(Component *trigger) { }
Button(switcher,
       _({
           .io  = &hw.io,
           .pin = &switcher_pin,

           .clock = &clk.state.time,

           .type            = BUTTON_TOGGLE,
           .bounce_delay_ms = 1000,

           .onToggle = switcher_toggle,
           .onRelease = switcher_released,
           .onPress = switcher_pressed
       }));

/* Push button for led blinking */
pin_t  pusher_pin = hw_pin(D, 0);
void   pusher_toggle(Component *trigger) { }
void   pusher_pressed(Component *trigger) { }
void   pusher_released(Component *trigger) { }
Button(pusher,
       _({
           .io  = &hw.io,
           .pin = &pusher_pin,

           .clock = &clk.state.time,

           .type            = BUTTON_PUSH,
           .bounce_delay_ms = 100,

           .onToggle = pusher_toggle,
           .onRelease = pusher_released,
           .onPress = pusher_pressed
       }));


test(long_push) {
    // Event-loop
    loop(clk, switcher)
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
    test_assert(enabled == false, "Should disabled at first");
    test_assert(Button_isPressed(&switcher) == false, "Switcher unpushed at first");
    test_assert(Button_isPressed(&pusher) == false, "Pusher unpushed at first");
    test_assert(hw.io.get(&switcher_pin) == hw.io.get(&pusher_pin), "Pins should be equal %d = %d", hw.io.get(&switcher_pin), hw.io.get(&pusher_pin));
    test_assert(hw.io.get(&pusher_pin) == 0, "Pusher pin sholud be 0, not %d at start", hw.io.get(&pusher_pin));

    usleep(1000);

    hw.io.on(&switcher_pin);
    usleep(1000);
    test_assert(hw.io.get(&pusher_pin) == 0, "Pusher pin sholud be 0, not %d after swither on", hw.io.get(&pusher_pin));
    test_assert(hw.io.get(&switcher_pin) != hw.io.get(&pusher_pin), "Pins should be different");

    usleep(500000);
    test_assert(hw.io.get(&switcher_pin) == 1, "Switcher pin sholud be 1, not %d after 500 ms button press", hw.io.get(&pusher_pin));
    test_assert(hw.io.get(&pusher_pin) == 0, "Pusher pin sholud be 0, not %d before swither off", hw.io.get(&pusher_pin));

    hw.io.off(&switcher_pin);
    test_assert(hw.io.get(&switcher_pin) == 0, "Switcher pin sholud be 0, not %d after 500 ms button release", hw.io.get(&pusher_pin));
    test_assert(enabled == false, "Should disabled after half sec");
    test_assert(hw.io.get(&pusher_pin) == 0, "Pusher pin sholud be 0, not %d after swither off", hw.io.get(&pusher_pin));

    hw.io.on(&switcher_pin);
    usleep(1100000);
    test_assert(enabled == true, "Should enabled after switch for 1 sec");
    test_assert(hw.io.get(&switcher_pin) == 1, "Switcher pin should be 1, not %d after second 1 sec button press", hw.io.get(&pusher_pin));

    hw.io.off(&switcher_pin);
    usleep(10);
    test_assert(hw.io.get(&pusher_pin) == 0, "Pusher pin sholud be 0, not %d before swither off", hw.io.get(&pusher_pin));
    test_assert(Button_isPressed(&pusher) == false, "Pusher unpushed after switcher manipulation");

    hw.io.on(&pusher_pin);
    usleep(10000);
    test_assert(Button_isPressed(&pusher) == false, "Pusher unpushed after 10 ms");
    usleep(110000);
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
