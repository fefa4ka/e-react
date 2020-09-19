#pragma once

#include <Calendar.h>

typedef struct _event {
  unsigned int   timeout_us;
  unsigned int   created;
  void           (*callback)(void *args);
  void           *args;
} event;

typedef struct _event_queue {
    unsigned char    size;
    unsigned char    capacity;
    event            *events;
    Component        *scheduler;
} event_queue;

typedef struct {
    timer_handler       *timer;

    rtc_datetime_t      *time;
    event_queue         *queue; 

    void (*onEventHappened)(Component *instance);
    void (*onEventEqueued)(Component *instance);
    void (*onEventScheduled)(Component *instance);
} Scheduler_blockProps;

typedef struct {
    bool         (*enqueue)(Component *scheduler, unsigned int timeout_us, void (*callback)(void *args), void *args);
    event        (*dequeue)(Component *scheduler);
    bool         scheduled;
    event        next_event;
} Scheduler_blockState;

    
React_Header(Scheduler);


#define Scheduler_enqueue(instance, ...) \
    ((Scheduler_blockState *)((instance)->state))->enqueue(instance, ##__VA_ARGS__)

#define Scheduler_dequeue(instance) \
    ((Scheduler_blockState *)((instance)->state))->dequeue(instance)
