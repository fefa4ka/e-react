#pragma once

#include "hash.h"
#include <signal.h>
#include <time.h>

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


#include "component.h""

#define REACT_PROFILER_COUNT(func)                                             \
    if (current_scope) {                                                       \
        current_scope->func += 1;                                              \
    } else {                                                                   \
        calls.func += 1;                                                       \
    }

#define REACT_PROFILER_TICK(component, stage)                                  \
    clock_t begin, end, passed;                                                \
    current_scope = &(component)->instance.calls.stage;                        \
    begin         = clock()

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
    vcd_init();                                                                \
    while (EVAL(MAP(loop_, __VA_ARGS__)) step())


extern struct hash_table scope;
extern struct HAL_calls *current_scope;
extern clock_t           cpu_total;
extern struct HAL_calls  calls;
extern bool              stop;


void         sighandler(int sig);
unsigned int frame_depth();
bool         dump_usage();
uint64_t     step();