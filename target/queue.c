#include "config/device.h"

#include <Scheduler.h>

#include "hal/virtual.h"
#include <time.h>
#include <stdio.h>

VirtualPin motor_pin_hw = {
    .port = 'B',
    .number = 3
};

#define HW Virtual_HAL 

typedef struct
{
    rtc_datetime_t   time;
    event_queue      scheduler; 


} device_state_t ;

device_state_t state = {
    .time = {0},
    .scheduler = {
        .size = 0,
        .capacity = 5
    },
};

/* Application handlers */
void timer_forever(Component *trigger)
{
    HW.io.flip(&motor_pin_hw);
}

unsigned int random_number(int min_num, int max_num)
    {
        int result = 0, low_num = 0, hi_num = 0;

        if (min_num < max_num)
        {
            low_num = min_num;
            hi_num = max_num + 1; // include max_num in output
        } else {
            low_num = max_num + 1; // include max_num in output
            hi_num = min_num;
        }

        srand(time(NULL));
        result = (rand() % (hi_num - low_num)) + low_num;
        return result;
    }
int main(void) {
    // Define React components
    react_define(Time, datetime);
    react_define(Scheduler, scheduler);
    HW.io.out(&motor_pin_hw);

    // Allocate memeory for events
    event events[state.scheduler.capacity];
    state.scheduler.events = events;
    state.scheduler.scheduler = &scheduler;

    // Timer component, for event management and time counting
    react (Time) {
        .timer = &(HW.timer),
        .time = &state.time
    } to (datetime);

    react (Scheduler) {
        .timer = &(HW.timer),
        .time = &state.time,
        .queue = &state.scheduler,
    } to (scheduler);

    Scheduler_enqueue(state.scheduler.scheduler, 100, &timer_forever, &scheduler);
    Scheduler_enqueue(state.scheduler.scheduler, 200, &timer_forever, &scheduler);
    Scheduler_enqueue(state.scheduler.scheduler, 10, &timer_forever, &scheduler);
    Scheduler_enqueue(state.scheduler.scheduler, 250, &timer_forever, &scheduler);
    Scheduler_enqueue(state.scheduler.scheduler, 20, &timer_forever, &scheduler);
    event closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 1: %d\r\n", closest.timeout_us);
    Scheduler_enqueue(state.scheduler.scheduler, 110, &timer_forever, &scheduler);
    closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 2: %d\r\n", closest.timeout_us);
    Scheduler_enqueue(state.scheduler.scheduler, 200, &timer_forever, &scheduler);
    closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 3: %d\r\n", closest.timeout_us);

    closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 4: %d\r\n", closest.timeout_us);
    closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 5: %d\r\n", closest.timeout_us);
    closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 6: %d\r\n", closest.timeout_us);
    closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 7: %d\r\n", closest.timeout_us);

    closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 8: %d\r\n", closest.timeout_us);
    closest = Scheduler_dequeue(state.scheduler.scheduler);
    printf("Last 9: %d\r\n", closest.timeout_us);
    return 0;
}


