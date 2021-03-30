#include "unit.h"

#include <Timer.h>
#define  _Timer_date

/* Datetime couting */
Timer(timer, &hw.timer, 0);
struct calendar now;


test(ticks) {
    timer.props.calendar = &now;

    loop(timer) {}
}


void ticks()
{
    test_assert(timer.state.time.timestamp == 0, "Timer shoud be 0 at start");
    usleep(1100000);
    test_assert(timer.state.time.timestamp == 1, "Timer shoud be 1");
    usleep(1100000);
    test_assert(timer.state.time.timestamp == 2, "Timer shoud be 2");
    usleep(1100000);
    test_assert(timer.state.time.timestamp == 3, "Timer shoud be 3");

    test_assert(now.year == 1970, "Year should be 1970");
}

