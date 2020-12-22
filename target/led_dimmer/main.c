#include "config.h"

unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device state = { .time          = { 0 },
                        .sensor        = SENSOR_POTENTIOMETER,
                        .brightness    = 0,
                        .angle         = 0,
                        .output_buffer = { output_buffer, BUFFER_SIZE },
                        .input_buffer  = { input_buffer, BUFFER_SIZE },
                        .led_pin       = hw_pin (B, 1) };

/* Application handlers */
void
log_string (char *message)
{
    rb_write_string (&state.output_buffer, message);
}

void
log_num (char *message, int number)
{
    rb_write_string (&state.output_buffer, message);
    rb_write_string (&state.output_buffer, itoa (number));
}

void
sensor_readed (Component *trigger)
{
    AtDC_blockState *adc_state = (AtDC_blockState *)trigger->state;

    if (state.sensor == SENSOR_POTENTIOMETER) {
        state.sensor = SENSOR_SOLAR_POWER;
        // NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax -
        // OldMin)) + NewMin
        state.angle = adc_state->value * 45 / 256;

    } else if (state.sensor == SENSOR_SOLAR_POWER) {
        state.brightness = adc_state->value;
        state.sensor     = SENSOR_POTENTIOMETER;
    }
}

void
log_sensors (Component *trigger)
{
    log_num ("\r\nLight: ", state.brightness);
    log_num ("\r\nPOT: ", state.angle);
}


int
main (void)
{
    // Define React components
    Time (datetime);
    PWM (led);
    AtDC (sensor);
    UART (serial);

    // Event-loop
    while (true) {
        // Timer component, for event management and time counting
        react (Time, datetime,
               _ ({ .timer    = &hw.timer,
                    .time     = &state.time,
                    .onSecond = log_sensors }));

        react (UART, serial,
               _ ({
                   .uart      = &hw.uart,
                   .baudrate  = BAUDRATE,
                   .tx_buffer = &state.output_buffer,
                   .rx_buffer = &state.input_buffer,
               }));

        react (AtDC, sensor,
               _ ({ .adc      = &hw.adc,
                    .channel  = &state.sensor,
                    .onChange = sensor_readed }));

        react (PWM, led,
               _ ({ .io         = &hw.io,
                    .pin        = &state.led_pin,
                    .frequency  = 120,
                    .duty_cycle = state.brightness,
                    .time       = &state.time }));
    }

    return 0;
}
