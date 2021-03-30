#pragma once

#include <component.h>

#define Timer(instance, timer_handler, timestamp)                              \
    define(Timer, instance, _({.timer = timer_handler}),                       \
           _({.time = {timestamp}}))
#define Timer_time(instance) React_State(Timer, instance, time)
#define Timer_set(instance, timestamp)                                         \
    {                                                                          \
        Time_state_t *state   = instance.state;                                \
        state->time.timestamp = timestamp;                                     \
    }
#define Timer_getYear(instance) Timer_time(instance).timestamp / 31536000 + 1970

struct Timer {
    unsigned long timestamp; /* Unix timestamp in seconds */
    unsigned int  ms;        /* Tick counter in ms */
    unsigned long us;        /* Tick counter in us */
    unsigned int  step_us;   /* Tick length in us */
};

struct calendar {
    int   sec;    /* seconds after the minute [0-60] */
    int   min;    /* minutes after the hour [0-59] */
    int   hour;   /* hours since midnight [0-23] */
    int   mday;   /* day of the month [1-31] */
    int   mon;    /* months since January [0-11] */
    int   year;   /* years since 1900 */
    int   wday;   /* days since Sunday [0-6] */
    int   yday;   /* days since January 1 [0-365] */
    int   isdst;  /* Daylight Savings Time flag */
    long  gmtoff; /* offset from CUT in seconds */
    char *zone;   /* timezone abbreviation */
};

typedef struct {
    timer_handler *  timer;
    struct calendar *calendar;
    void (*onSecond)(Component *instance);
} Timer_props_t;

typedef struct {
    struct Timer time;

    unsigned int ms;     /* Collect ms for seconds tick */
    unsigned int us;     /* Collect us for ms tick */
    unsigned int tick;   /* Last timer value */
    unsigned int passed; /* Passed from last check in timer ticks */
} Timer_state_t;

React_Header(Timer);
#ifdef _Timer_date
void Timer_date(unsigned long seconds, struct calendar *tm);
#endif
