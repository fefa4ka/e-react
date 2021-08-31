#pragma once

#include <component.h>

#define Clock(instance, timer_handler, timestamp)                              \
    define(Clock, instance, _({.timer = timer_handler}),                       \
           _({.time = {timestamp}}))
#define Clock_time(instance) React_State(Clock, instance, time)
#define Clock_set(instance, timestamp)                                         \
    {                                                                          \
        Time_state_t *state   = instance.state;                                \
        state->time.timestamp = timestamp;                                     \
    }
#define Clock_getYear(instance) Clock_time(instance).timestamp / 31536000 + 1970

struct Clock {
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
} Clock_props_t;

typedef struct {
    struct Clock time;

    unsigned int ms;     /* Collect ms for seconds tick */
    unsigned int us;     /* Collect us for ms tick */
    unsigned int tick;   /* Last timer value */
    unsigned int passed; /* Passed from last check in timer ticks */
} Clock_state_t;

React_Header(Clock);
#ifdef _Clock_date
void Clock_date(unsigned long seconds, struct calendar *tm);
#endif
