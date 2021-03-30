#include "unit.h"

#include <Timer.h>
#include <Scheduler.h>

/* Timer and callback scheduler */
Timer(timer, &hw.timer, TIMESTAMP);
Scheduler(scheduler, 15, _({.timer = &hw.timer}));


int trigger_count = 0;
void scheduler_trigger(void *trigger) {
    trigger_count++;
    log_info("Scheduled event #%d - %d", trigger_count, *(int *)trigger);
}


test(queue_push_pop) {
    loop(timer, scheduler) {}
}

void queue_push_pop() {
    int a = 1000;
    int b = 2000;
    int c = 1500;
    int d = 5500;
    int e = 200;
    int f = 600;
    int g = 700;
    int h = 800;

    Scheduler_enqueue(&scheduler, a, scheduler_trigger, &a);
    Scheduler_enqueue(&scheduler, b, scheduler_trigger, &b);
    Scheduler_enqueue(&scheduler, c, scheduler_trigger, &c);
    Scheduler_enqueue(&scheduler, d, scheduler_trigger, &d);
    Scheduler_enqueue(&scheduler, e, scheduler_trigger, &e);

    sleep(2);
}
