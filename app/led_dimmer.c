#include <ADC.h>
#include <PWM.h>
#include <Clock.h>

#ifdef ARCH_STM8L
    #define SENSOR_POTENTIOMETER 15
    #define SENSOR_SOLAR_POWER   13
#else
    #define SENSOR_POTENTIOMETER 0
    #define SENSOR_SOLAR_POWER   1
#endif

/* Application handlers */
void sensor_readed(Component *);

/* Application state */
struct device {
    int            sensor;
    unsigned short brightness;
} state = {
    .sensor     = SENSOR_POTENTIOMETER,
    .brightness = 0,
};


Clock(clk, &hw.timer, TIMESTAMP);

/* Sensor reader */
ADC(sensor, _({
                .adc      = &hw.adc,
                .channel  = &state.sensor,
                .onChange = sensor_readed,
            }));

/* Led blinker */
pin_t led_pin = hw_pin(B, 1);
PWM(led, {0},
    _({
        .io    = &hw.io,
        .pin   = &led_pin,
        .clock = &clk.state.time
    }));

///
/// \brief Save converted ADC value
void sensor_readed(Component *adc)
{
    state.brightness = 100 - ((ADC_t *)adc)->state.value / 10;
}

int main(void)
{
    // Event-loop

    loop(clk, sensor)
    {
        apply(PWM, led,
              _({
                  .frequency  = 120,
                  .duty_cycle = state.brightness,
              }));
    }

    return 0;
}
