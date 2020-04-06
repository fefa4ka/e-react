#ifndef Time_block_h
#define Time_block_h

#include "../../react/component.h"
#include "../../hal/api.h"

typedef struct _rtc_datetime
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
} rtc_datetime;

typedef struct _rtc_event {
  unsigned int   timeout_us;
  unsigned int   created;
  bool           repeated;
  void           (*callback)(void *args);
  void           *args;
} rtc_event;

typedef struct _event_queue {
    unsigned char    size;
    unsigned char    capacity;
    rtc_event        *events;
} event_queue;


typedef struct {
    timer_handler       *timer;

    rtc_datetime        *time;
    event_queue         *scheduler;

    void (*onSecond)(Component *instance);
    void (*onMinute)(Component *instance);
    void (*onHour)(Component *instance);
    void (*onDay)(Component *instance);
} Time_blockProps;

typedef struct {
    unsigned int tick;
    unsigned int passed;

    void         (*enqueue)(Component *time, unsigned int timeout_us, void (*callback)(void *args), void *args);
    rtc_event   *next_event;
} Time_blockState;

    
React_Header(Time);


#define Time_enqueue(instance, ...) \
    ((Time_blockState *)((instance)->state))->enqueue(instance, ##__VA_ARGS__)

#endif
