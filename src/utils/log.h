#ifndef utils_log_h
#define utils_log_h

#include "common.h"

enum eLogLevel {
    info,
    warning,
    error
};

void Log(enum eLogSubSystem, enum eLogLevel, char *message);
void LogWithNum(enum eLogSubSystem, enum eLogLevel, char *message, int number);

#endif

