#include "routines.h"


void log_string(char *message) 
{
    while(*message) {
        rb_write(&state.output_buffer, *(message++));
    }; 
}

void log_num(char *message, int number) 
{
    log_string(message);
    log_string(itoa(number));
} 


void print_command(Component *trigger) 
{
    log_string("\r\n$ > ");
    log_string(state.command);
    log_string("\r\n");
}

void print_version(Component *trigger) 
{
    log_num("\r\ne-react ver. 0.", VERSION);
    log_num(".", BUILD_NUM);
    log_string("\r\n"); 
}

void print_time(Component *trigger) 
{
    log_string("Timestamp: ");
    log_string(utoa(state.time.time_us));
    log_string(" us | ");
    log_string(utoa(state.time.second));
    log_string(" s ");
    log_string(utoa(state.time.millisecond));
    log_string(" ms ");
    log_string(utoa(state.time.microsecond));
    log_string(" us");
    log_string("\r\n");
}


void read_command(Component *trigger) 
{
    unsigned char *command = state.command;
    unsigned char data;

    while(rb_read(&state.input_buffer, &data) == eErrorNone && data) {
        *command++ = data;
    }
    *--command = 0;
}

