#pragma once

#include <Clock.h>

/*
 *  ┌─────┐ Scheduler_enqueue() ┌─────┐  get closest  ┌─────────────────┐
 *  │event├────────────────────►│QUEUE├──────────────►│Fire or set Timer│
 *  └─────┘                     └─────┘               └─────────────────┘
 *
 *  ┌───────────◄──┐      ┌────────────┐       ┌─────────┐      ┌─────────┐
 *  │ ShouldUpdate ├─────►│ WillUpdate ├──────►│ Release │      │ Release │        Release
 *  └──────────────┘      └────────────┘       └─────────┘      └─────────┘
 *   if in queue           Prioritity queue     Fire event
 *   new closest event     and clean timer      or set timer
 */

#define Scheduler(instance, scheduler_capacity, props)                         \
    struct event instance##_events[scheduler_capacity];                        \
    define(Scheduler, instance, _(props),                                      \
           _({                                                                 \
               .queue = {.events   = instance##_events,                        \
                         .capacity = scheduler_capacity},                      \
           }))

/* Define with Timer handler */
#define Scheduler_timer_handler(instance)                                      \
    extern timer_handler instance##_timer_handler;                             \
    void                 instance##_timer_init(void *args)                     \
    {                                                                          \
        if (!instance##_timer_handler.get) {                                   \
            instance##_timer_handler.get = instance.props.timer->get;          \
            instance##_timer_handler.off = instance.props.timer->off;          \
        }                                                                      \
        instance.props.timer->init(args);                                      \
    }                                                                          \
    void instance##_timer_set(unsigned int timeout_ms,                         \
                              void (*callback)(void *args), void *args)        \
    {                                                                          \
        Scheduler_enqueue(&instance, timeout_ms, callback, args);              \
    }                                                                          \
    timer_handler instance##_timer_handler                                     \
        = {.init = instance##_timer_init, .set = instance##_timer_set}

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

    struct Clock *clock; /* TODO: For longer events that timer can't handle */
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
