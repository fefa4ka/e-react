#ifndef Scheduler_block_h
#define Scheduler_block_h

#include "../Time/Time.h"

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

    rtc_datetime        *time;
    event_queue         *queue;
} Scheduler_blockProps;

typedef struct {
    void         (*enqueue)(Component *scheduler, unsigned int timeout_us, void (*callback)(void *args), void *args);
    event        next_event;
} Scheduler_blockState;

    
React_Header(Scheduler);


#define Scheduler_enqueue(instance, ...) \
    ((Scheduler_blockState *)((instance)->state))->enqueue(instance, ##__VA_ARGS__)

#endif
