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

static const long hextable[]
    = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,
        6,  7,  8,  9,  -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1 };

/**
 * @brief convert a hexidecimal string to a signed long
 * will not produce or process negative numbers except
 * to signal error.
 *
 * @param hex without decoration, case insensitive.
 *
 * @return -1 on error, or result (max (sizeof(long)*8)-1 bits)
 */
long
hexdec (unsigned const char *hex)
{
    long ret = 0;
    while (*hex && ret >= 0) {
        ret = (ret << 4) | hextable[*hex++];
    }
    return ret;
}

void print_shell(Component *trigger) 
{
    state.command = 0;

    log_string("\r\n$ > ");
}

void print_version(Component *trigger) 
{
    log_num("\r\ne-react ver. 0.", VERSION);
    log_num(".", BUILD_NUM);
    log_string("\r\n"); 
}

void print_time(Component *trigger) 
{
    log_string("\r\nTimestamp: ");
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


void read_symbol(Component *trigger) { 
    rb_write(&state.output_buffer, state.input_buffer.data[state.input_buffer.write - 1]);
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

