#pragma once

#include <Calendar.h>

typedef struct _event {
  unsigned int   timeout_us;
  unsigned int   created;
  void           (*callback)(void *args);
  void           *args;
} event;

struct events_queue {
    unsigned char    size;
    unsigned char    capacity;
    event            *events;
    Component        *scheduler;
};

typedef struct {
    timer_handler       *timer;

    rtc_datetime_t      *time;
    struct events_queue *queue; 

    void (*onEventHappened)(Component *instance);
    void (*onEventEqueued)(Component *instance);
    void (*onEventScheduled)(Component *instance);
} Scheduler_blockProps;

typedef struct {
    bool         scheduled;
    event        next_event;
} Scheduler_blockState;

    
React_Header(Scheduler);

bool Scheduler_enqueue(Component *instance, unsigned int timeout_us, void (*callback)(void *args), void *args);
bool Scheduler_dequeue(Component *instance, event *); 

#define Scheduler(instance, queue) \
    component(Scheduler, instance); \
    event events[queue.capacity]; \
    queue.events = events; \
    queue.scheduler = &scheduler

