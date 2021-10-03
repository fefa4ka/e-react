#include "Scheduler.h"

/* Priority queue navigation */
#define LEFT(x)   (2 * (x) + 1)
#define RIGHT(x)  (2 * (x) + 2)
#define PARENT(x) ((x) / 2)


void event_callback(void *instance);
int  event_compare(unsigned int now, struct event *a, struct event *b);
void event_heapify(struct events_queue *scheduler, unsigned int now,
                   unsigned char idx);
void event_prioritify(struct events_queue *queue, unsigned int now,
                      struct event *new_event);

/**
 * \brief    Add event to queue
 */
bool Scheduler_enqueue(Scheduler_Component *self, unsigned int timeout_us,
                       void (*callback)(void *args), void *    args)
{
    struct events_queue *queue = &self->state.queue;
    timer_handler *      timer = self->props.timer;
    unsigned int         now   = timer->get();

    if (queue->size == queue->capacity || timeout_us == 0) {
        /* Not enought space in queue */
        return false;
    }

    struct event new_event = {timeout_us, now, callback, args};

    event_prioritify(queue, now, &new_event);

    return true;
}

/**
 * \brief    Add event to queue
 */
void event_prioritify(struct events_queue *queue, unsigned int now,
                      struct event *new_event)
{
    unsigned char index  = queue->size;
    struct event *events = queue->events;

    queue->events[index] = *new_event;
    queue->size += 1;

    /* Sorting */
    struct event swap = {0};
    while (index > 0
           && event_compare(now, &events[index], &events[PARENT(index)]) > 0) {
        swap                  = events[index];
        events[index]         = events[PARENT(index)];
        events[PARENT(index)] = swap;
        index                 = PARENT(index);
    }
}

/**
 * \brief    Fire callback and turn off timer
 */
void event_callback(void *instance)
{
    Scheduler_Component *self = instance;
    self->props.timer->off();

    struct event triggered_event = {0};
    triggered_event              = self->state.next_event;

    triggered_event.callback(triggered_event.args);
    struct event null_event = {0};
    self->state.next_event  = null_event;
}

/**
 * \brief    If result is positive event A is close than B
 */
int event_compare(unsigned int now, struct event *a, struct event *b)
{
    int          a_passed  = now - a->created;
    unsigned int a_timeout = a->timeout_us << 1;
    if (a_passed < 0) {
        a_passed = 65535 - a->created + now;
    }

    int          b_passed  = now - b->created;
    unsigned int b_timeout = b->timeout_us << 1;
    if (b_passed < 0) {
        b_passed = 65535 - b->created + now;
    }

    int a_remain = a_passed - a_timeout;
    int b_remain = b_passed - b_timeout;

    /*
    a->created = now;
//    a_passed = a_passed >> 1;
  //  b_passed = b_passed >> 1;
    if (a_timeout > a_passed) {
        a->timeout_us -= a_passed >> 1;
    } else {
        log_pin (B, 5);
        a->timeout_us = 0;
    }

    b->created = now;
    if (b_timeout > b_passed) {
        b->timeout_us -= b_passed >> 1;
    } else {
        log_pin (B, 0);
        b->timeout_us = 0;
    }
    */

    return a_remain - b_remain;
}

/**
 * \brief    Reorder queue with event priority
 */
void event_heapify(struct events_queue *scheduler, unsigned int now,
                   unsigned char idx)
{
    struct event  swap   = {0};
    struct event *events = scheduler->events;

    unsigned char l_idx, r_idx, lrg_idx;

    l_idx = LEFT(idx);
    r_idx = RIGHT(idx);

    /* Left child exists, compare left child with its parent */
    if (l_idx < scheduler->size
        && event_compare(now, &events[l_idx], &events[idx]) > 0) {
        lrg_idx = l_idx;
    } else {
        lrg_idx = idx;
    }

    /* Right child exists, compare right child with the largest element */
    if (r_idx < scheduler->size
        && event_compare(now, &events[r_idx], &events[lrg_idx]) > 0) {
        lrg_idx = r_idx;
    }

    /* At this point largest element was determined */
    if (lrg_idx != idx) {
        /* Swap between the index at the largest element */
        swap            = events[lrg_idx];
        events[lrg_idx] = events[idx];
        events[idx]     = swap;
        /* Heapify again */
        event_heapify(scheduler, now, lrg_idx);
    }
}

/**
 * \brief    Get closest event
 */
bool Scheduler_dequeue(Scheduler_Component *self, struct event *dequed_event)
{
    struct events_queue *queue      = &self->state.queue;
    timer_handler *      timer      = self->props.timer;
    struct event *       events     = queue->events;
    unsigned int         last_index = queue->size - 1;

    /* Empty queue */
    struct event null_event = {0};
    if (queue->size == 0) {
        *dequed_event = null_event;
        return false;
    }

    struct event closest_event = {0};
    closest_event              = events[0];

    events[0] = events[last_index];
    queue->size -= 1;

    if (queue->size == 0) {
        /* Now queue empty */
        events[0] = null_event;

        *dequed_event = closest_event;
        return true;
    } else {
        events[last_index] = null_event;
    }

    /* Queue reordering because last slot swapped */
    if (queue->size > 1) {
        event_heapify(queue, timer->get(), 0);
    }

    *dequed_event = closest_event;
    return true;
}

/**
 * \brief     Clean pointers
 */
willMount(Scheduler)
{
    /* comment if used with timer */
    // props->timer->init(ptr);
    struct event null_event = {0};
    state->next_event       = null_event;
}

/**
 * \brief     Update if no sheduled event
 *            or new closest event available
 */
shouldUpdate(Scheduler)
{
    // If queue empty, there are nothing to do
    if (state->queue.size == 0) {
        return false;
    }

    struct event *closest_event = &state->queue.events[0];

    // If current closest timeout as next event timeout, everything is setup
    //    if(closest_event->timeout_us != 0
    //            && closest_event->timeout_us == state->next_event.timeout_us)
    //            {

    /* If next event closest that in queue */
    if (closest_event->callback && state->next_event.callback
        && event_compare(props->timer->get(), &state->next_event,
                         closest_event)) {
        return false;
    }

    return true;
}

/**
 * \brief     Get closest event
 */
willUpdate(Scheduler)
{
    if (state->next_event.callback) {
        /* Event allready scheduled, back it to queue */
        props->timer->off();
        event_prioritify(&state->queue, props->timer->get(),
                         &state->next_event);
    }

    /* Get new event with highest priority */
    Scheduler_dequeue(self, &state->next_event);
}

/**
 * \brief    Fire or schedule event callback
 */
release(Scheduler)
{
    if (state->next_event.callback == 0) {
        /* No events */
        return;
    }

    unsigned int now            = props->timer->get();
    int          passed         = now - state->next_event.created;
    unsigned int timeout        = state->next_event.timeout_us << 1;
    unsigned int scheduled_tick = now + timeout;

    /* Handle timer overflow */
    if (passed < 0) {
        passed = 65535 - state->next_event.created + now;
    }

    scheduled_tick -= passed;

    if (passed > timeout
        || (scheduled_tick > now && props->timer->get() > scheduled_tick)
        || (now > scheduled_tick && scheduled_tick > props->timer->get())) {
        /* Scheduled event allready should happen */
        event_callback(self);
    } else {
        /* Schedule callback by timer, probably interrupt will be used */
        props->timer->set(scheduled_tick, event_callback, self);
    }
}

didMount(Scheduler) {}
didUpdate(Scheduler) {}


