#include <Clock.h>

Clock(clk, &hw.timer, TIMESTAMP);
pin_t tick_pin = hw_pin(B, 0);

void print_time(Component *trigger) {
    hw.io.flip(&tick_pin);
}

int main(void) {
    clk.props.onSecond = &print_time;

    loop(clk);
}
