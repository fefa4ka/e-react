#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define DEBUG 1

#define log_print(type, message, ...) do { if(DEBUG) printf("%s [" type "] " message "\n", get_formatted_time(), ##__VA_ARGS__); } while(0)
#define debug_print(type, message, ...) log_print(type, message " (%s:%d)\n", ##__VA_ARGS__, __FILE__, __LINE__)


#define log_info(message, ...) log_print("INFO", message, ##__VA_ARGS__)
#define log_ok(message, ...) log_print("OK", message, ##__VA_ARGS__)
#define log_error(message, ...) debug_print("\e[1m\e[31mERROR\e[39m\e[0m", message, ##__VA_ARGS__)

#define test_assert(test, message, ...)    \
    if (!(test))                           \
    {                                      \
        log_error(message, ##__VA_ARGS__); \
        return;                    \
    }

#define test_define_(test_method) void test_method();
#define test_define(...) EVAL(MAP(test_define_, __VA_ARGS__))

#define test_execute_(test_method) \
    pthread_t test_method##_thread; \
    int test_method##_thread_id; \
    pthread_create(&test_method##_thread, NULL, *test_method, (void *)test_method##_thread_id);

#define test_execute(...) EVAL(MAP(test_execute_, __VA_ARGS__))

#define test_wait_(test_method) pthread_join(test_method##_thread, NULL);
#define test_wait(...) EVAL(MAP(test_wait_, __VA_ARGS__))

#define test(...) \
test_define(__VA_ARGS__); \
void *program(void *ptr); \
int \
main (void) \
{ \
    pthread_t program_thread; \
    int program_thread_id; \
    pthread_create(&program_thread, NULL, *program, (void *)program_thread_id); \
    test_execute(__VA_ARGS__); \
    test_wait(__VA_ARGS__); \
    free_pins(); \
    pthread_kill(program_thread, 2); \
    return 0; \
} \
void *program(void *ptr) \

#define test_loop(number) for(int i = 0; i < number; i++)
#define test_loop_delay(number, order) for(int i = 0; i < number; i++, usleep(rand() % 10 * order))

// Returns the local date/time formatted as 2014-03-19 11:11:52
char* get_formatted_time(void) {

    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Must be static, otherwise won't work
    static char _retval[20];
    strftime(_retval, sizeof(_retval), "%Y-%m-%d %H:%M:%S", timeinfo);

    return _retval;
}
