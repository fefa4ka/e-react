#include <AtDC.h>
#include <Calendar.h>
#include <IO.h>
#include <PWM.h>
#include <UART.h>

#define BAUDRATE 9600

#ifdef ARCH_STM8L
    #define SENSOR_POTENTIOMETER 15
    #define SENSOR_SOLAR_POWER   13
#else
    #define SENSOR_POTENTIOMETER 0
    #define SENSOR_SOLAR_POWER   1
#endif

#define BUFFER_SIZE 128

struct device
{
    struct rtc_datetime time;
    int                 sensor;
    unsigned short      brightness;
    unsigned short      angle;
    struct ring_buffer  output_buffer;
    struct ring_buffer  input_buffer;
    pin_t               led_pin;
};
