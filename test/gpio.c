#include "unit.h"

#include <IO.h>


/* Indicator */
IO(led);
pin_t led_pin = hw_pin(B, 0);

bool  enabled    = false;
pin_t sensor_pin = hw_pin(B, 1);
void sensor_read(Component *trigger) {
    IO_Component *sensor = (IO_Component *)trigger;
    enabled = sensor->state.level;
}
IO_new(sensor, _({.io    = &hw.io,
                  .pin   = &sensor_pin,
                  .mode  = IO_INPUT,
                  .onChange = sensor_read }));

pin_t led_control_pin = hw_pin(D, 1);
pin_t debug_pin       = hw_pin(D, 0);

test(toggle_led, toggle_sensor)
{
    // Event-loop
    loop(sensor) {
        apply(IO, led,
              _({.io    = &hw.io,
                 .pin   = &led_pin,
                 .mode  = IO_OUTPUT,
                 .level = hw.io.get(&led_control_pin)}));
    }
}

void toggle_sensor()
{
    test_assert(hw.io.get(&sensor_pin) == 0, "Sensor pin should be off");
    test_assert(enabled == false, "Enabled should be false");

    hw.io.on(&sensor_pin);
    usleep(100);
    test_assert(enabled == true, "Enabled should be true");
    test_assert(hw.io.get(&sensor_pin) == 1, "Sensor pin should be on");

    hw.io.off(&sensor_pin);
    usleep(10);
    test_assert(hw.io.get(&sensor_pin) == 0, "Sensor pin should be off");
    test_assert(enabled == false, "Enabled should be false");
}

void toggle_led()
{
    test_assert(hw.io.get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw.io.get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw.io.get(&debug_pin) == 0, "Debug pin should be off");

    hw.io.on(&led_control_pin);
    usleep(10);
    test_assert(hw.io.get(&led_control_pin) == 1,
                "Led control pin should be on");
    test_assert(hw.io.get(&debug_pin) == 0, "Debug pin should be off");
    test_assert(hw.io.get(&led_pin) == 1, "Led pin should be on");

    hw.io.off(&led_control_pin);
    usleep(10);
    test_assert(hw.io.get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw.io.get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw.io.get(&debug_pin) == 0, "Debug pin should be off");

    hw.io.on(&debug_pin);
    usleep(10);
    test_assert(hw.io.get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw.io.get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw.io.get(&debug_pin) == 1, "Debug pin should be on");

    hw.io.on(&led_control_pin);
    usleep(10);
    test_assert(hw.io.get(&led_pin) == 1, "Led pin should be on");
    test_assert(hw.io.get(&led_control_pin) == 1, "Led pin should be on");
    test_assert(hw.io.get(&debug_pin) == 1, "Debug pin should be on");

    hw.io.flip(&led_control_pin);
    usleep(10);
    test_assert(hw.io.get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw.io.get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw.io.get(&debug_pin) == 1, "Debug pin should be on");

    hw.io.flip(&debug_pin);
    usleep(10);
    test_assert(hw.io.get(&led_pin) == 0, "Led pin should be off");
    test_assert(hw.io.get(&led_control_pin) == 0,
                "Led control pin should be off");
    test_assert(hw.io.get(&debug_pin) == 0, "Debug pin should be off");
}
