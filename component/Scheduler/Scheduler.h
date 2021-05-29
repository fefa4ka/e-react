#pragma once

#include <Timer.h>

#define Scheduler(instance, scheduler_capacity, props)                         \
    struct event instance##_events[scheduler_capacity];                        \
    define(Scheduler, instance, _(props),                                      \
           _({                                                                 \
               .queue = {.events   = instance##_events,                        \
                         .capacity = scheduler_capacity},                      \
           }))

///
/// \brief Scheduled event with callback and args
///
struct event {
    unsigned int timeout_us;
    unsigned int created;
    void (*callback)(void *args);
    void *args;
};

///
/// \brief Priority queue
///
struct events_queue {
    unsigned char size;
    unsigned char capacity;
    struct event *events;
};

typedef struct {
    timer_handler *timer; /* Use timer callback on tick comparasion */

    struct Timer *time; /* TODO: For longer events that timer can't handle */
} Scheduler_props_t;

typedef struct {
    bool                scheduled;  /* Is next event timer set */
    struct event        next_event; /* Scheduled event */
    struct events_queue queue;      /* Priority queue */
} Scheduler_state_t;

React_Header(Scheduler);

bool Scheduler_enqueue(Scheduler_Component *instance, unsigned int timeout_us,
                       void (*callback)(void *args), void *        args);
bool Scheduler_dequeue(Scheduler_Component *instance, struct event *);
