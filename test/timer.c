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
    test_assert(clk.state.time.timestamp == 0, "Clock should be 0 at start, but %d", clk.state.ms);
    sleep(1);
    test_assert(clk.state.time.timestamp == 1, "Clock should be 1, but %d (clk_ms = %d)", clk.state.time.timestamp, clk.state.ms);
    sleep(1);
    test_assert(clk.state.time.timestamp == 2, "Clock should be 2, but %d (clk_ms = %d)", clk.state.time.timestamp, clk.state.ms);
    sleep(1);
    test_assert(clk.state.time.timestamp == 3, "Clock should be 3, but %d (clk_ms = %d)", clk.state.time.timestamp, clk.state.ms);
    sleep(1);
    test_assert(clk.state.time.timestamp == 4, "Clock should be 4, but %d (clk_ms = %d)", clk.state.time.timestamp, clk.state.ms);
    sleep(1);
    test_assert(clk.state.time.timestamp == 5, "Clock should be 5, but %d (clk_ms = %d)", clk.state.time.timestamp, clk.state.ms);

    test_assert(now.year == 1970, "Year should be 1970, but %d", now.year);
}

