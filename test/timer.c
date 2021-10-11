#include "unit.h"

#include <Clock.h>
#define  _Clock_date

/* Datetime couting */
Clock(clk, &hw.timer, 0);
struct calendar now;


test(ticks) {
    clk.props.calendar = &now;

    loop(clk) {}
}


void ticks()
{
    test_assert(clk.state.time.timestamp == 0, "Clock shoud be 0 at start");
    usleep(1100000);
    test_assert(clk.state.time.timestamp == 1, "Clock shoud be 1");
    usleep(1100000);
    test_assert(clk.state.time.timestamp == 2, "Clock shoud be 2");
    usleep(1100000);
    test_assert(clk.state.time.timestamp == 3, "Clock shoud be 3");

    test_assert(now.year == 1970, "Year should be 1970");
}

