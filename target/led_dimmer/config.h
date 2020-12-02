#include <AtDC.h>
#include <IO.h>
#include <Calendar.h>
#include <PWM.h>
#include <UART.h>

#define BAUDRATE 9600
enum sensor_channel {
    potentiomenter = 0,
    solar_panel = 1
};

#define BUFFER_SIZE 128

struct device
{
    struct rtc_datetime time;
    enum sensor_channel sensor;
    unsigned short      brightness;
    unsigned short      angle;
    struct ring_buffer  output_buffer;
    struct ring_buffer  input_buffer;
    pin_t               led_pin;
};
