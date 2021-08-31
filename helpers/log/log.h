#pragma once

#include "circular.h"
#include "common.h"
#include <Time.h>

extern const int version;
extern void log_str(char *message);
void log_buffer(char *message, struct ring_buffer_s *buffer);
void log_num(char *message, int number);
void log_version();
void log_time(rtc_datetime_t *time);
