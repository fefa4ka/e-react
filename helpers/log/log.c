#include "logger.h"


void log_buffer(char *message, struct ring_buffer_s *buffer)
{
    while(*message) {
        rb_write(buffer, *(message++));
    }; 
}

void log_num(char *message, int number)
{
    log_str(message);
    log_str(itoa(number));
} 

void log_version()
{
    log_num("\r\nbuild ", BUILD_NUM);
    log_str("\r\n"); 
}

void log_time(rtc_datetime_t *time)
{
    log_str("Timestamp: ");
    log_str(utoa(time->time_us));
    log_str(" us | ");
    log_str(utoa(time->second));
    log_str(" s ");
    log_str(utoa(time->millisecond));
    log_str(" ms ");
    log_str(utoa(time->microsecond));
    log_str(" us");
    log_str("\r\n");
}
