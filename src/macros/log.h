//
//  macros.h
//  naive
//
//  Created by Alexandr Kondratyev on 14/09/2018.
//  Copyright © 2018 alexander. All rights reserved.
//

#ifndef macros_log_h
#define macros_log_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <util/delay.h>

// Logging

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define log_print(type, message, ...) printf(type " " message "\n", ##__VA_ARGS__)
#define debug_print(type, message, ...) log_print(type, message " %s:%d\n", ##__VA_ARGS__, __FILENAME__, __LINE__)

#define log_info(message, ...) log_print("i", message, ##__VA_ARGS__)
#define log_error(message, ...) debug_print("\e[1m\e[31mE\e[39m\e[0m", "#%d " message, errno, ##__VA_ARGS__)
#define log_warning(message, ...) log_print("\e[33mW\e[39m", "#%d " message, errno, ##__VA_ARGS__)



#endif

