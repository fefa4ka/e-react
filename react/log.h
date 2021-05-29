#ifndef utils_log_h
#define utils_log_h

#include "common.h"

enum log_system {
    LOG_SYSTEM_COMMON,
    LOG_SYSTEM_REACT, 
    LOG_SYSTEM_COMPONENT, 
    LOG_SYSTEM_EVENT_LOOP
};

enum log_level {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR 
};

void log(enum log_system, enum log_level, char *message);
void log_list(enum log_system, enum log_level, char *message[]);
void log_int(enum log_system, enum log_level, char *message, int number);

#ifdef ARCH_AVR
#define log_pin(port, pin)                                                    \
    DDR##port |= (1 << pin);                                                  \
    PORT##port ^= (1 << pin);                                                 \
    PORT##port ^= (1 << pin);
#endif

#ifdef ARCH_x86
#define log_pin(port, pin) printf("Debug PORT##port.%d", pin)
#endif

#endif
