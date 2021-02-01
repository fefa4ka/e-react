#pragma once

#include <component.h>

struct rtc_datetime
{
  unsigned int   year;
  unsigned char  month;
  unsigned char  date;
  unsigned char  hour;
  unsigned char  minute;
  unsigned char  second;
  unsigned int   millisecond;
  unsigned int   microsecond;
  unsigned char  day_of_week;
  unsigned int   time_ms;
  unsigned long  time_us;
  unsigned int   step_us;
};
typedef struct rtc_datetime rtc_datetime_t;


typedef struct {
    timer_handler       *timer;

    rtc_datetime_t      *time;

    void (*onSecond)(Component *instance);
    void (*onMinute)(Component *instance);
    void (*onHour)(Component *instance);
    void (*onDay)(Component *instance);
} Time_blockProps;

typedef struct {
    unsigned int tick;
    unsigned int passed;
} Time_blockState;

    
React_Header(Time);
#define Time(instance) component(Time, instance)

