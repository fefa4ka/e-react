#ifndef utils_log_h
#define utils_log_h

#include "common.h"

enum eLogLevel {
    info,
    warning,
    error
};

void Log(enum eLogSubSystem, enum eLogLevel, char *message);
void LogWithList(enum eLogSubSystem, enum eLogLevel, char *message[]);
void LogWithNum(enum eLogSubSystem, enum eLogLevel, char *message, int number);

#define log_pin(port, pin) \
    DDR##port |= (1 << pin); \
    PORT##port ^= (1 << pin); \
    PORT##port ^= (1 << pin);

#endif

