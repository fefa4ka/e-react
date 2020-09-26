#pragma once 

#include "config.h"
#include <component.h>

extern struct device state;

void log_string(char *message);
void log_num(char *message, int number);
void print_command(Component *trigger);
void print_version(void *args);
void read_command(Component *trigger);
void print_time(void *args);

