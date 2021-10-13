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
    uint32_t timestamp; /* Unix timestamp in seconds */
    uint16_t ms;        /* Tick counter in ms */
    uint32_t us;        /* Tick counter in us */
    uint16_t step_us;   /* Tick length in us */
};

struct calendar {
    uint8_t  sec;    /* seconds after the minute [0-60] */
    uint8_t  min;    /* minutes after the hour [0-59] */
    uint8_t  hour;   /* hours since midnight [0-23] */
    uint8_t  mday;   /* day of the month [1-31] */
    uint8_t  mon;    /* months since January [0-11] */
    uint16_t  year;  /* years since 1900 */
    uint8_t  wday;   /* days since Sunday [0-6] */
    uint16_t yday;   /* days since January 1 [0-365] */
    uint8_t  isdst;  /* Daylight Savings Time flag */
    uint16_t gmtoff; /* offset from CUT in seconds */
    uint8_t *zone;   /* timezone abbreviation */
};

typedef struct {
    timer_handler *  timer;
    struct calendar *calendar;
    void (*onSecond)(Component *instance);
} Clock_props_t;

typedef struct {
    struct Clock time;

    uint16_t ms;     /* Collect ms for seconds tick */
    uint16_t us;     /* Collect us for ms tick */
    uint16_t tick;   /* Last timer value */
    uint16_t passed; /* Passed from last check in timer ticks */
} Clock_state_t;

React_Header(Clock);
#ifdef _Clock_date
void Clock_date(unsigned long seconds, struct calendar *tm);
#endif
