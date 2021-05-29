#include <IO.h>
#include "unit.h"

IO(get);
pin_t get_pin   = hw_pin(D, 4);

IO(set);
pin_t set_pin   = hw_pin(D, 5);

bool value = false;
bool is_changed, is_get_low, is_get_high = false;


void get_changed(Component *trigger) {
    IO_Component *io = trigger;
    
    pin_t *pin = io->props.pin;

    is_changed = true;
    log_info("Pin state changed");
}

void get_low(Component *trigger) {
    is_get_low = true;
    log_info("Pin state is low");
}

void get_high(Component *trigger) {
    is_get_high = true;
    log_info("Pin state is high");
}


test(level_change_and_callbacks)
{
    // Event-loop
    while (true) {
        apply(IO, get,
              _({
                  .io       = &hw.io,
                  .pin      = &get_pin,
                  .mode     = IO_INPUT,
                  .onChange = get_changed,
                  .onLow    = get_low,
                  .onHigh   = get_high,
              }));


        apply(IO, set,
              _({
                  .io    = &hw.io,
                  .pin   = &set_pin,
                  .mode  = IO_OUTPUT,
                  .level = value
              }));
    }
}


void level_change_and_callbacks()
{
    
    log_info("Check IO level setup");
    value = true;
    sleep(1);
    test_assert(hw.io.get(&set_pin) == true, "Pin should be in high state");

    value = false;
    sleep(1);
    test_assert(hw.io.get(&set_pin) == false, "Pin should be in low state");

    sleep(1);
    test_assert(is_changed, "onChange should happen on mount");
    test_assert(is_get_low == false, "onLow shouldn't happen");
    test_assert(is_get_high == false, "onHigh shouldn't happen");

    log_info("Detecting pin state changing");
    hw.io.on(&get_pin);
    sleep(1);
    test_assert(is_changed, "onChange should happen");
    test_assert(is_get_high, "onHigh should happen");
    test_assert(is_get_low == false, "onLow shouldn't happen");
    
    is_get_high = false;
    is_changed = false;

    hw.io.off(&get_pin);
    sleep(1);
    test_assert(is_changed, "onChange should happen");
    test_assert(is_get_low, "onLow should happen");
    test_assert(is_get_high == false, "onHigh shouldn't happen");
}


