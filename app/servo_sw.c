#include <ADC.h>
#include <Button.h>
#include <Scheduler.h>
#include <Servo.h>

#include <stdbool.h>

struct device {
    enum { SENSOR_POTENTIOMETER = 0, SENSOR_SOLAR_POWER = 1 } sensor;
    unsigned short thrust;
    unsigned short angle;

    bool motors_enabled;
} state = {
    .sensor = SENSOR_POTENTIOMETER,
    .thrust = 0,
    .angle  = 0,

    .motors_enabled = true,
};


/* Timer and callback scheduler */
Clock(clock, &hw.timer, TIMESTAMP);
Scheduler(scheduler, 15, _({.timer = &hw.timer}));
Scheduler_timer_handler(scheduler);


/* Motors */
#define SERVO_SPEED 10
#define Servo_setup(instance, pin_definition)                                  \
    pin_t instance##_pin = pin_definition;                                     \
    Servo(instance, {0},                                                       \
          _({                                                                  \
              .io    = &hw.io,                                                 \
              .pin   = &instance##_pin,                                        \
              .timer = &scheduler_timer_handler,                               \
          }))
#define Servo_drive(instance, servo_angle)                                     \
    react(Servo, instance,                                                     \
          _({                                                                  \
              .speed = state.motors_enabled ? SERVO_SPEED : 0,                 \
              .angle = servo_angle,                                            \
          }))

Servo_setup(engine, hw_pin(B, 1));
Servo_setup(left_actuator, hw_pin(B, 2));
Servo_setup(right_actuator, hw_pin(B, 3));


/* Engine start/stop switch */
pin_t switcher_pin = hw_pin(D, 1);
void  switch_motor(Component *trigger)
{
    state.motors_enabled = !state.motors_enabled;
}
Button(switcher, _({
                     .io  = &hw.io,
                     .pin = &switcher_pin,

                     .type = BUTTON_PUSH_PULLUP,

                     .clock = &clock.state.time,
                     .timer = &scheduler_timer_handler,
                     .bounce_delay_ms = 100,

                     .onRelease = switch_motor,
                 }));


/* Sensor reader channels in sequence */
void sensor_readed(Component *trigger)
{
    ADC_Component *adc   = trigger;
    unsigned int    value = adc->state.value * 45 / 256;

    if (state.sensor == SENSOR_POTENTIOMETER) {
        state.angle  = value;
        state.sensor = SENSOR_SOLAR_POWER;

        Servo_drive(left_actuator, state.angle);
        Servo_drive(right_actuator, 180 - state.angle);
    } else {
        state.thrust = value;
        state.sensor = SENSOR_POTENTIOMETER;

        Servo_drive(engine, state.thrust);
    }
}
ADC(sensor, _({
                 .adc     = &hw.adc,
                 .channel = &state.sensor,

                 .onChange = sensor_readed,
             }));


int main(void)
{
    use(switcher, sensor);

    loop(clock, scheduler);

    return 0;
}
