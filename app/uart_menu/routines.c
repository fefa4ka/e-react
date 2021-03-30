#include "routines.h"

static int counter_index = 0;

void log_string(char *message) 
{
    Serial_write_string(&uart, message);
}

void log_num(char *message, long number) 
{
    log_string(message);
    log_string(itoa(number));
} 
// function to convert decimal to binary
void log_binary(char n)
{
    // array to store binary number
    int binaryNum[8];
 
    // counter for binary array
    int i = 0;
    while (n > 0) {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }
 
    // printing binary array in reverse order
    for (int j = 8; j >= 0; j--)
        Serial_write(&uart, (binaryNum[j] == 1) ? '1' : '0');
}

void log_nl() {
    char newline[] = "\r\n";
    Serial_write_string(&uart, newline);
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
    *command = 0;

    log_nl();
    log_string("$ > ");
}

void print_command_not_found(Component *trigger) {
    log_nl();
    log_string(command);
    log_string(" not found.");

    print_shell(trigger);
}

void print_counter(Component *trigger) 
{
    counter_index += 1;
    log_nl();
    log_num("counter: ", counter_index);
}

void print_version(Component *trigger) 
{
    log_nl();
    log_num("e-react ver. ", BUILD_NUM);
}

void print_memory(char *address) {
    char *address_hex = address + 5;
    void *address_pointer = hexdec(address_hex);

    log_memory(address_pointer);
}

void dump_memory(char *request) {
    char *address_hex = request + 5;
    address_hex[4] = '\0';
    void *address_pointer = hexdec(address_hex);

    int size = itoa(address_hex + 5);

    for(int i = 0; i < size; i++) {
        log_memory(address_pointer++);
    }
}

void write_memory(char *request) {
    char *address_hex = request + 5;
    address_hex[4] = '\0';
    void *address_pointer = hexdec(address_hex);

    char *value_hex = address_hex + 5;
    void *value = hexdec(value_hex);
}

void log_memory(char *data)
    log_nl();
    log_string(address_hex);
    log_string(": ");
    log_binary(*data);
}

void print_time(Component *trigger) 
{
    struct Timer *time = &Timer_time(&timer);
    log_nl();
    log_string("Timestamp: ");
    log_string(utoa(time->timestamp));
    log_string(" s ");
    log_string(utoa(time->us));
    log_string(" us | ");
}

