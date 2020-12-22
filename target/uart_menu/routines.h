#pragma once 

#include "config.h"
#include <component.h>

extern struct device state;

void log_string(char *message);
void log_num(char *message, int number);
void print_shell(Component *trigger);
void print_version(Component *trigger);
void read_command(Component *trigger);
void read_symbol(Component *trigger);
void print_time(Component *trigger);

