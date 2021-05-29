#pragma once

#include <Serial.h>
#include <Timer.h>
#include <component.h>

#define BAUDRATE            9600
#define BUFFER_SIZE         128
#define COMMAND_BUFFER_SIZE 32

extern unsigned char    command[COMMAND_BUFFER_SIZE];
extern Serial_Component uart;
extern Timer_Component timer;

void log_string(char *message);
void log_num(char *message, long number);
void print_shell(Component *trigger);
void print_command_not_found(Component *trigger);
void print_version(Component *trigger);
void print_memory(char *address);
void print_counter(Component *trigger);
void print_time(Component *trigger);
