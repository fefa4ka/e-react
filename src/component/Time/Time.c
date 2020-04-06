#include "Time.h"

#define LEFT(x) (2 * (x) + 1)
#define RIGHT(x) (2 * (x) + 2)
#define PARENT(x) ((x) / 2)

void event_callback(void *instance);
int event_compare(unsigned int now, rtc_event *a, rtc_event *b);
rtc_event dequeue(Component *instance); 
void event_heapify(event_queue *scheduler, unsigned int now, unsigned char idx); 
void enqueue(Component *instance, unsigned int timeout_us, void (*callback)(void *args), void *args);

void 
event_callback(void *instance) {
    React_Load(Time, (Component *)instance);
    event_queue *scheduler = TimeProps->scheduler;
    timer_handler *timer = TimeProps->timer;

    // Get next event
    // and remove event from queue
    rtc_event event = dequeue((Component *)instance); 

    // Create new event if repeated
    
    // Create new interrupt if event exists
    if(scheduler->size > 0) {
        rtc_event *closest_event = &scheduler->events[0];
        unsigned int now = timer->get();
        int passed = now - closest_event->created;
        unsigned int timeout = closest_event->timeout_us << 1;

        if(passed < 0) {
            passed = 65535 - closest_event->created + now;
        }

        if(passed > timeout) {
            event_callback(instance);
        } else {
            timer->set(timeout - passed, event_callback, instance);
        }
    } else {
        // Timer off if no events
        timer->off();
    }

    event.callback(event.args);
}

int
event_compare(unsigned int now, rtc_event *a, rtc_event *b) {
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

    return a_remain - b_remain;
}

void
event_heapify(event_queue *scheduler, unsigned int now, unsigned char idx) {
    rtc_event swap = {0};
    rtc_event *events = scheduler->events;

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

rtc_event
dequeue(Component *instance) {
    log_pin(D, 7);
    React_Load(Time, instance);
    event_queue *scheduler = TimeProps->scheduler;
    timer_handler *timer = TimeProps->timer;
    rtc_event *events = scheduler->events;

    if(scheduler->size == 0) {
        return (rtc_event){0};
    }

    rtc_event closest_event = events[0];

    events[0] = events[scheduler->size - 1];
    scheduler->size -= 1;

    event_heapify(scheduler, timer->get(), 0);

    return closest_event;
}

void
enqueue(Component *instance, unsigned int timeout_us, void (*callback)(void *args), void *args) {
    log_pin(B, 4);
    React_Load(Time, instance);
    event_queue *scheduler = TimeProps->scheduler;
    rtc_event *events = scheduler->events;
    timer_handler *timer = TimeProps->timer;
    unsigned int now = timer->get();


    rtc_event *closest_event = &events[0];
    rtc_event *new_event = &scheduler->events[scheduler->size];
    new_event->callback = callback;
    new_event->args = args;
    new_event->timeout_us = timeout_us;
    new_event->created = now;
    

    // Use priority queue
    unsigned char index = scheduler->size;
    scheduler->size++;
    
    rtc_event swap = {0};
    while(index > 0 && event_compare(now, &events[index], &events[PARENT(index)]) < 0) {

       swap = events[index];
       events[index] = events[PARENT(index)];
       events[PARENT(index)] = swap;
       index = PARENT(index);
    }
    
    // If new timer earlier that already exists, replace it
    // < 0 Event with lower remain time have higher priority
    if(event_compare(now, closest_event, new_event) < 0 || scheduler->size == 1) {

        timer->set(now + (timeout_us << 1), event_callback, instance);
    }
}

void
repeat(Component *instance, unsigned int timeout_us, void (*callback)(void *args), void *args) {
    // Ad event
}

willMount(Time_block) {
    void *ptr;
    props->timer->init(ptr);
    state->enqueue = enqueue;
}

shouldUpdate(Time_block) {
    return true;  
}

willUpdate(Time_block) { 
    unsigned int tick = props->timer->get();

    state->passed = tick - state->tick;
    if(state->passed < 0) {
        state->passed = 65535 - state->tick + tick;
    }

    state->tick = tick;
}

release(Time_block) {
    unsigned int us_passed = props->timer->usFromTicks(state->passed);
    props->time->microsecond += us_passed; // state->passed;//134;//ms_passed;
    props->time->time_us += us_passed;
    
    if(props->time->microsecond > 1000) {
        unsigned int ms_passed = props->time->microsecond / 1000;
        props->time->time_ms += ms_passed;
        props->time->millisecond += ms_passed; 
        props->time->microsecond -= ms_passed * 1000;

        if(props->time->millisecond > 1000) {
            unsigned int s_passed = props->time->millisecond / 1000;
            props->time->second += s_passed; 
            props->time->millisecond -= s_passed * 1000;

            if(props->time->second >= 60) {
                unsigned int m_passed = props->time->second / 60;
                props->time->minute += m_passed;
                props->time->second -= m_passed * 60;

                if(props->onSecond) props->onSecond(self);

                if(props->time->minute >= 60) {
                    unsigned int h_passed = props->time->minute / 60;
                    props->time->hour += h_passed;
                    props->time->minute -= h_passed * 60;

                    if(props->onMinute) props->onMinute(self);

                    if(props->time->hour >= 24) {
                        unsigned int d_passed = props->time->hour / 24;
                        props->time->date += d_passed;
                        props->time->hour -= d_passed * 24;

                        if(props->onHour) props->onHour(self);

                        // TODO: month, year
                    }
                }
            }
        }

    }
}

didMount(Time_block) { }
didUnmount(Time_block) { }

didUpdate(Time_block) {
  
}


React_Constructor(Time_block);
