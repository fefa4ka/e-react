//
//  macros.h
//  naive
//
//  Created by Alexandr Kondratyev on 14/09/2018.
//  Copyright © 2018 alexander. All rights reserved.
//

#ifndef macros_error_h
#define macros_error_h

#include "log.h"

// Error Handling
#define push_error(message, ...) { log_error(message, ##__VA_ARGS__); errno = 0; goto error; }

#define check(expression, message, ...) do { if(DEBUG && !(expression)) push_error(message, ##__VA_ARGS__) } while(0)
#define check_memory_print(variable, message, ...) check((variable), "Out of memory. " message, ##__VA_ARGS__)
#define check_memory(variable) check_memory_print(variable, "")
#define check_debug(expression, message, ...) if(!(expression)) { debug(message, ##__VA_ARGS__); }
#define sentinel(message, ...) push_error(message, ##__VA_ARGS__)

#endif

