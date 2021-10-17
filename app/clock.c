#include <Clock.h>

Clock(clk, &hw.timer, TIMESTAMP);

void print_time(Component *trigger) {
    printf("Time: %ld\n", clk.state.time.timestamp);
}

int main(void) {
    clk.props.onSecond = &print_time;

    loop(clk);
}
