#include "Scheduler.h"

// Priority queue navigation
#define LEFT(x) (2 * (x) + 1)
#define RIGHT(x) (2 * (x) + 2)
#define PARENT(x) ((x) / 2)


void event_callback(void *instance);
int event_compare(unsigned int now, event *a, event *b);
event dequeue(Component *instance); 
void event_heapify(event_queue *scheduler, unsigned int now, unsigned char idx); 
void event_prioritify(event_queue *queue, unsigned int now, event *new_event);
void enqueue(Component *instance, unsigned int timeout_us, void (*callback)(void *args), void *args);

void
enqueue(Component *instance, unsigned int timeout_us, void (*callback)(void *args), void *args) {
    log_pin(D, 6);
    React_Load(Scheduler, instance);
    event_queue *queue = SchedulerProps->queue;
    timer_handler *timer = SchedulerProps->timer;
    unsigned int now = timer->get();

    event new_event = {
        timeout_us,
        now,
        callback,
        args
    };

    // Use priority queue
    event_prioritify(queue, now, &new_event);
}

void
event_prioritify(event_queue *queue, unsigned int now, event *new_event) {
    // Use priority queue
    unsigned char index = queue->size;
    event *events = queue->events;

    queue->events[index] = *new_event;
    queue->size++;
    
    event swap = {0};
    while(index > 0 && event_compare(now, &events[index], &events[PARENT(index)]) > 0) {
       swap = events[index];
       events[index] = events[PARENT(index)];
       events[PARENT(index)] = swap;
       index = PARENT(index);
    }
}

void 
event_callback(void *instance) {
    React_Load(Scheduler, (Component *)instance);

    event triggered_event = SchedulerState->next_event; 
    
    triggered_event.callback(triggered_event.args);
    SchedulerState->next_event = (event){0, 0, 0, 0};
    
    SchedulerProps->timer->off();
}

// If result is positive event A is close than B
int
event_compare(unsigned int now, event *a, event *b) {
    int a_passed = now - a->created;
    if(a_passed < 0) {
        a_passed = 65535 - a->created + now;
    }

    int b_passed = now - b->created;
    if(b_passed < 0) {
        b_passed = 65535 - b->created + now;
    }

    int a_remain = a_passed - (a->timeout_us << 1);
    int b_remain = b_passed - (b->timeout_us << 1);

    return a_remain - b_remain; //b_remain - a_remain; 
}

void
event_heapify(event_queue *scheduler, unsigned int now, unsigned char idx) {
    event swap = {0};
    event *events = scheduler->events;

    unsigned char l_idx, r_idx, lrg_idx;

    l_idx = LEFT(idx);
    r_idx = RIGHT(idx);

    /* Left child exists, compare left child with its parent */
    if (l_idx < scheduler->size && event_compare(now, &events[l_idx], &events[idx]) > 0) {
        lrg_idx = l_idx;
    } else {
        lrg_idx = idx;
    }

    /* Right child exists, compare right child with the largest element */
    if (r_idx < scheduler->size && event_compare(now, &events[r_idx], &events[lrg_idx]) > 0) {
        lrg_idx = r_idx;
    }

    /* At this point largest element was determined */
    if (lrg_idx != idx) {
        /* Swap between the index at the largest element */
        swap = events[lrg_idx];
        events[lrg_idx] = events[idx];
        events[idx] = swap;
        /* Heapify again */
        event_heapify(scheduler, now, lrg_idx);
    }
}

event
dequeue(Component *instance) {
    log_pin(D, 7);
    React_Load(Scheduler, instance);
    event_queue *scheduler = SchedulerProps->queue;
    timer_handler *timer = SchedulerProps->timer;
    event *events = scheduler->events;

    if(scheduler->size == 0) {
        return (event){0, 0, 0, 0};
    }

    event closest_event = events[0];

    events[0] = events[scheduler->size - 1];
    scheduler->size -= 1;

    event_heapify(scheduler, timer->get(), 0);

    return closest_event;
}

willMount(Scheduler_block) {
    //void *ptr;
    //props->timer->init(ptr);
    state->enqueue = enqueue;
    state->next_event = (event){0, 0, 0, 0};
}

shouldUpdate(Scheduler_block) {
    // If queue empty, there are nothing to do
    if(props->queue->size == 0) {
        return false;
    }
    
    event *closest_event = &props->queue->events[0];
    // If current closest timeout as next event timeout, everything is setup
    if(closest_event->timeout_us == state->next_event.timeout_us) {
        return false;
    }


    return true;
}

willUpdate(Scheduler_block) {
    if(state->next_event.timeout_us != 0) {
        log_pin(D, 4);
        event_prioritify(props->queue, props->timer->get(), &state->next_event);
    }

    state->next_event = dequeue(self);
}

release(Scheduler_block) { 
    if(state->next_event.timeout_us == 0) {

        return;
    }

    unsigned int now = props->timer->get();
    int passed = now - state->next_event.created;
    unsigned int timeout = state->next_event.timeout_us << 1;

    if(passed < 0) {
        passed = 65535 - state->next_event.created + now;
    }

    if(passed > timeout) {
        event_callback(self);
    } else {
        props->timer->set(now + timeout - passed, event_callback, self);
    }
}

didMount(Scheduler_block) { }
didUnmount(Scheduler_block) { }
didUpdate(Scheduler_block) { }


React_Constructor(Scheduler_block);
