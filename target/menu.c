#include "config/device.h"

#include <Scheduler.h>
#include <Menu.h>

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
    unsigned char    command[32];
    event_queue      scheduler; 


} device_state_t ;

device_state_t state = {
    .time = {0},
    .command = {0},
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

void print_time(void *args) 
{
    printf("Time");
}
void print_version(void *args) 
{
    printf("Version");
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
    react_define(Menu, tty);
    HW.io.out(&motor_pin_hw);

    // Allocate memeory for events
    event events[state.scheduler.capacity];
    state.scheduler.events = events;
    state.scheduler.scheduler = &scheduler;
    menu_command_t commands[3] = {
                { 
                    .command = "time", 
                    .callback = print_time },
                { 
                    .command = "version", 
                    .callback = print_version },
                { 0 }
            };

    strcpy(state.command, "version");

    while(true) {
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

        react (Menu) {
            .menu = commands,
            .command = &state.command
        } to (tty);


    }


    return 0;
}


