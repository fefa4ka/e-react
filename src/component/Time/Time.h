#ifndef Time_block_h
#define Time_block_h

#include "../../react/component.h"
#include "../../hal/api.h"

typedef struct _rtc_datetime_24h_t
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
} rtc_datetime_24h_t;

typedef struct {
    timer_handler *timer;
    rtc_datetime_24h_t *time;

    void (*onEventHappen)(Component *instance);
} Time_blockProps;

typedef struct {
    unsigned int tick;
    unsigned int passed;
} Time_blockState;

React_Header(Time);

#endif
