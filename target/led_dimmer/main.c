#include "config.h"

unsigned char output_buffer[BUFFER_SIZE];
unsigned char input_buffer[BUFFER_SIZE];

struct device state = { .time          = { 0 },
                        .sensor        = potentiomenter,
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

    if (state.sensor == potentiomenter) {
        state.sensor = solar_panel;
        // NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax -
        // OldMin)) + NewMin
        state.angle = adc_state->value * 45 / 256;

    } else if (state.sensor == solar_panel) {
        state.brightness = adc_state->value;
        state.sensor     = potentiomenter;
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
        react (Time) { .timer    = &(hw.timer),
                       .time     = &state.time,
                       .onSecond = log_sensors 
        } to (datetime);

        react (UART) { .uart     = &(hw.uart),
                       .baudrate = BAUDRATE,
                       .tx_buffer = &state.output_buffer,
                       .rx_buffer = &state.input_buffer,
        } to (serial);

        react (AtDC) { .adc      = &(hw.adc),
                       .channel  = &(state.sensor),
                       .onChange = sensor_readed 
        } to (sensor);

        react (PWM) { .io         = &(hw.io),
                      .pin        = &state.led_pin,
                      .frequency  = 20,
                      .duty_cycle = state.brightness,
                      .time       = &state.time 
        } to (led);
    }

    return 0;
}
