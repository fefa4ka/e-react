#pragma once

#include "hash.h"
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MAX_TABLE_SIZE 10007 // Prime Number for hash table

#define REACT_PROFILER_STRUCT                                                  \
    char name[128];                                                            \
    struct {                                                                   \
        uint64_t willMount;                                                    \
        uint64_t nextProps;                                                    \
        uint64_t shouldUpdate;                                                 \
        uint64_t willUpdate;                                                   \
        uint64_t release;                                                      \
        uint64_t didMount;                                                     \
        uint64_t didUpdate;                                                    \
        uint64_t didUnmount;                                                   \
    } counter;                                                                 \
    struct {                                                                   \
        struct HAL_calls willMount;                                            \
        struct HAL_calls nextProps;                                            \
        struct HAL_calls shouldUpdate;                                         \
        struct HAL_calls willUpdate;                                           \
        struct HAL_calls release;                                              \
        struct HAL_calls didMount;                                             \
        struct HAL_calls didUpdate;                                            \
        struct HAL_calls didUnmount;                                           \
    } calls;                                                                   \
    struct {                                                                   \
        uint64_t willMount;                                                    \
        uint64_t nextProps;                                                    \
        uint64_t shouldUpdate;                                                 \
        uint64_t willUpdate;                                                   \
        uint64_t release;                                                      \
        uint64_t didMount;                                                     \
        uint64_t didUpdate;                                                    \
        uint64_t didUnmount;                                                   \
    } cpu;


#include "component.h"

#define REACT_PROFILER_COUNT_LOG(func, format, ...)                            \
    log_verbose("(%p:%d) " #func "" format, __builtin_return_address(0),       \
                frame_depth(), __VA_ARGS__);                                   \
    REACT_PROFILER_COUNTER(func)

#define REACT_PROFILER_COUNT(func)                                             \
    log_verbose("(%p:%d) " #func "()", __builtin_return_address(0),            \
                frame_depth());                                                \
    REACT_PROFILER_COUNTER(func)

#define REACT_PROFILER_COUNTER(func)                                           \
    if (current_scope) {                                                       \
        current_scope->func += 1;                                              \
    } else {                                                                   \
        calls.func += 1;                                                       \
    }

#define REACT_PROFILER_TICK(component, stage)                                  \
    clock_t begin, end, passed;                                                \
    current_scope = &(component)->instance.calls.stage;                        \
    log_verbose("%s." #stage, (component)->instance.name);                     \
    begin = clock()

#define REACT_PROFILER_TOCK(component, stage)                                  \
    end    = clock();                                                          \
    passed = end - begin;                                                      \
    cpu_total += passed;                                                       \
    (component)->instance.cpu.stage += passed;                                 \
    (component)->instance.counter.stage++;                                     \
    current_scope = NULL

#undef React_Define_Component
#define React_Define_Component(Type, instance_name)                            \
    {                                                                          \
        .stage = REACT_STAGE_DEFINED, .WillMount = Type##_willMount,           \
        .ShouldUpdate = Type##_shouldUpdate, .WillUpdate = Type##_willUpdate,  \
        .Release = Type##_release, .DidMount = Type##_didMount,                \
        .DidUpdate = Type##_didUpdate, .name = #Type " / " #instance_name      \
    }

#undef React_LifeCycle_Prepare
#define React_LifeCycle_Prepare(Type, instance, stage)                         \
    hw_isr_disable();                                                          \
    REACT_PROFILER_TICK((Type##_Component *)instance, stage)

#undef React_LifeCycle_Finish
#define React_LifeCycle_Finish(Type, instance, stage)                          \
    REACT_PROFILER_TOCK((Type##_Component *)instance, stage);                  \
    hw_isr_enable()

#undef loop
#define loop(...)                                                              \
    signal(SIGINT, sighandler);                                                \
    log_init();                                                                \
    vcd_init();                                                                \
    while (EVAL(MAP(loop_, __VA_ARGS__)) step())

#undef log_print
#ifdef DEBUG
    #define log_print(type, message, ...)                                      \
        log_printf("%s\t" type "\t%lld\t" message "\n",                        \
                   timer_formatted_time(), steps, ##__VA_ARGS__)
#else
    #define log_print(type, message, ...)
#endif


#define log_debug(message, ...)                                                \
    log_print("DEBUG", "(%s:%d) " message, __FILE__, __LINE__, ##__VA_ARGS__)


#define log_verbose(message, ...) log_print("VERBOSE", message, ##__VA_ARGS__)
#define log_info(message, ...)    log_print("INFO", message, ##__VA_ARGS__)
#define log_ok(message, ...)      log_print("OK", message, ##__VA_ARGS__)
#define log_error(message, ...)                                                \
    debug_print("\e[1m\e[31mERROR\e[39m\e[0m", message, ##__VA_ARGS__)

extern struct hash_table scope;
extern uint64_t          steps;
extern struct HAL_calls *current_scope;
extern clock_t           cpu_total;
extern struct HAL_calls  calls;
extern bool              stop;


void         sighandler(int sig);
unsigned int frame_depth();
bool         dump_usage();
uint64_t     step();
char *       timer_formatted_time(void);
unsigned int hash_component(char *word);
