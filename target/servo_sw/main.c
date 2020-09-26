#include "config/device.h"

struct device state = {
    .time = {0},
    .sensor = potentiomenter,
    .thrust = 0,
    .angle = 0,
    .motors_enabled = true,
    .scheduler = {
        .size = 0,
        .capacity = 15 
    },
};

void sensor_readed(Component *trigger) {
    // component_state(AtDc, trigger, adc_state);

    AtDC_blockState *adc_state = (AtDC_blockState *)trigger->state;
    
    state.delay = adc_state->value * 20; 
    state.angle = adc_state->value * 45 / 256; 
}

void switch_motor(Component *trigger) {
    state.motor_enabled = !state.motor_enabled;

    bool scheduled = Scheduler_enqueue(
        state.scheduler.scheduler, 
        state.delay, 
        delayed_message, trigger
    );
}

int main(void) {
    // Define React components
    component(Time, datetime);
    component(AtDC, sensor);
    component(Button, button);
    component(Scheduler, scheduler);
    component(Servo, left_actuator);
    component(Servo, right_actuator);
    component(Servo, engine);

    // Allocate memeory for events
    event events[state.scheduler.capacity];
    state.scheduler.events = events;
    state.scheduler.scheduler = &scheduler;

    // Event-loop
    while (true) { 
        // Timer component, for event management and time counting
        react (Time) {
            .timer = &(hw.timer),
            .time = &state.time
        } to (datetime);

        react (Scheduler) {
            .timer = &(hw.timer),
            .time = &state.time,
            .queue = &state.scheduler,
        } to (scheduler);

        react (AtDC) {
            .adc = &(hw.adc),
            .channel = &(state.sensor),
            .onChange = sensor_readed 
        } to (sensor);

        react (Button) {
            .io = &(HW.io),
            .pin = &state.switcher_pin,
            .type = toggle,
            .time = &state.time,
            .bounce_delay_ms = 100,
            .onRelease = button_release,
            .onToggle = switch_motor
        } to (switcher);

        react (Servo) {
            .io = &(HW.io),
            .pin = &state.left_actuator_pin,
            .scheduler = &scheduler,
            .speed = 10,
            .enabled = state.motors_enabled,
            .angle = state.angle
        } to (left_actuator);

        
        react (Servo) {
            .io = &(HW.io),
            .pin = &state.right_actuator_pin,
            .scheduler = &scheduler,
            .speed = 30,
            .enabled = state.motors_enabled,
            .angle = 180 - state.angle  
        } to (right_actuator);

        /*
        react (Servo) {
            .io = &(HW.io),
            .pin = &nervo_pin_hw,
            .scheduler = &scheduler,
            .speed = 15,
            .enabled = state.motor_enabled,
            .angle = 180 - state.angle  
        } to (nervo);
        */
    }

    return 0;
}


