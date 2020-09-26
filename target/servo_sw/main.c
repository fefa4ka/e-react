#include "config.h"

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
    // react_state(AtDc, trigger, adc_state);
    AtDC_blockState *adc_state = (AtDC_blockState *)trigger->state;
    
    if(state.sensor == potentiomenter) {
        state.angle = adc_state->value * 45 / 256; 
        state.sensor = solar_panel;
    } else {
        state.thrust = adc_state->value; 
        state.sensor = potentiomenter;
    }
}

void switch_motor(Component *trigger) {
    state.motors_enabled = !state.motors_enabled;
}

int main(void) {
    // Define React components
    Time(datetime);
    AtDC(sensor);
    Button(switcher);
    Scheduler(scheduler, state.scheduler);
    Servo(left_actuator);
    Servo(right_actuator);
    Servo(engine);

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
            .io = &(hw.io),
            .pin = &state.switcher_pin,
            .type = toggle,
            .time = &state.time,
            .bounce_delay_ms = 100,

            .onToggle = switch_motor
        } to (switcher);

        react (Servo) {
            .io = &(hw.io),
            .pin = &state.left_actuator_pin,
            .scheduler = &scheduler,
            .speed = 10,
            .enabled = state.motors_enabled,
            .angle = state.angle
        } to (left_actuator);

        
        react (Servo) {
            .io = &(hw.io),
            .pin = &state.right_actuator_pin,
            .scheduler = &scheduler,
            .speed = 30,
            .enabled = state.motors_enabled,
            .angle = 180 - state.angle  
        } to (right_actuator);

        /*
        react (Servo) {
            .io = &(hw.io),
            .pin = &state.engine_pin,
            .scheduler = &scheduler,
            .speed = 15,
            .enabled = state.motors_enabled,
            .angle = state.thrust 
        } to (nervo);
        */
    }

    return 0;
}


