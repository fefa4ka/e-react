#include <Scheduler.h>

#include <stdio.h>
#include <time.h>

pin_t motor_pin_hw = hw_pin(B, 1);

#define HW Virtual_HAL

struct device
{
    struct rtc_datetime    time;
    struct events_queue    scheduler; 
};


struct device state = {
    .time      = { 0 },
    .scheduler = { .size = 0, .capacity = 5 },
};

/* Application handlers */
void
timer_forever (void *trigger)
{
    hw.io.flip (&motor_pin_hw);
}

unsigned int
random_number (int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num) {
        low_num = min_num;
        hi_num  = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num  = min_num;
    }

    srand (time (NULL));
    result = (rand () % (hi_num - low_num)) + low_num;
    return result;
}

int
main (void)
{
    // Define React components
    int a =1;
    int b = 2;
    int c = 3;
    int d = 4;
    int f = 5;
    int g = 6;
    int h = 7;
    Time (datetime);
    Scheduler (scheduler, state.scheduler);

    hw.io.out (&motor_pin_hw);

    // Allocate memeory for events
    event events_heap[state.scheduler.capacity];
    state.scheduler.events    = events_heap;
    state.scheduler.scheduler = &scheduler;

    // Timer component, for event management and time counting
    react (Time, datetime, _ ({ .timer = &hw.timer, .time = &state.time }));

    react (Scheduler, scheduler, _({
        .timer = &hw.timer,
        .time = &state.time,
        .queue = &state.scheduler,
    }));

    Scheduler_enqueue(state.scheduler.scheduler, 100, timer_forever, &a);
    Scheduler_enqueue(state.scheduler.scheduler, 200, timer_forever, &b);
    Scheduler_enqueue(state.scheduler.scheduler, 10, timer_forever, &c);
    Scheduler_enqueue(state.scheduler.scheduler, 250, timer_forever, &d);
    Scheduler_enqueue(state.scheduler.scheduler, 20, timer_forever, &f);
    event closest = {0};
    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 1: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);
    Scheduler_enqueue(state.scheduler.scheduler, 110, timer_forever, &g);
    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 2: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);
    Scheduler_enqueue(state.scheduler.scheduler, 200, timer_forever, &h);
    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 3: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);

    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 4: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);
    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 5: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);
    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 6: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);
    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 7: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);

    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 8: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);
    Scheduler_dequeue(state.scheduler.scheduler, &closest);
    printf("Last 9: %d - %d\r\n", closest.timeout_us, *(int*)closest.args);
    return 0;
}
