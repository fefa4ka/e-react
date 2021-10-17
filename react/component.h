#pragma once

#include "macros.h"
#include <hal.h>
#include <stdbool.h>

/*
 *
 *        ┌───────┐  willMount   ┌────────┐   shouldUpdate?   ┌────────┐
 *        │DEFINED├─────────────►│RELEASED├──────────────────►│PREPARED├──┐
 *        └───────┘  release     └────────┘   willUpdate      └────────┘  │
 *                   didMount         ▲                        release    │
 *                                    ├───────────────────────────────────┘
 *                                    ▼                        didUpdate
 *                               ┌─────────┐
 *                               │UNMOUNTED│
 *                               └─────────┘
 */

typedef struct {
    enum {
        REACT_STAGE_BLOCKED,
        REACT_STAGE_DEFINED,
        REACT_STAGE_PREPARED,
        REACT_STAGE_RELEASED,
        REACT_STAGE_UNMOUNTED
    } stage;

    void (*WillMount)(void *instance, void *next_props);

    bool (*ShouldUpdate)(void *instance, void *next_props);
    void (*WillUpdate)(void *instance, void *next_props);

    void (*Release)(void *instance);

    void (*DidMount)(void *instance);
    void (*DidUpdate)(void *instance);
    void (*DidUnmount)(void *instance);
#ifdef REACT_PROFILER
    char name[128];
    struct {
        uint64_t willMount;
        uint64_t nextProps;
        uint64_t shouldUpdate;
        uint64_t willUpdate;
        uint64_t release;
        uint64_t didMount;
        uint64_t didUpdate;
        uint64_t didUnmount;
    } counter;
    struct {
        struct HAL_calls willMount;
        struct HAL_calls nextProps;
        struct HAL_calls shouldUpdate;
        struct HAL_calls willUpdate;
        struct HAL_calls release;
        struct HAL_calls didMount;
        struct HAL_calls didUpdate;
        struct HAL_calls didUnmount;
    } calls;
    struct {
        uint64_t willMount;
        uint64_t nextProps;
        uint64_t shouldUpdate;
        uint64_t willUpdate;
        uint64_t release;
        uint64_t didMount;
        uint64_t didUpdate;
        uint64_t didUnmount;
    } cpu;
#endif
} Component;

#ifdef REACT_PROFILER
    #define MAX_TABLE_SIZE 10007 // Prime Number
    #include <time.h>
unsigned int             frame_depth();
bool                     dump_usage();
extern struct HAL_calls *       current_scope;
extern clock_t           cpu_total;
uint64_t                 step();
extern struct hash_table scope;
extern struct HAL_calls  calls;
#endif

bool Stage_Component(Component *instance, void *next_props);
bool React_Component(Component *instance, void *next_props);

// Component Creation
#define React_Instance_State(Type, instance)                                   \
    ((Type##_state_t *)((instance)->state))
#define React_Instance_Props(Type, instance)                                   \
    ((Type##_props_t *)((instance)->props))
#define React_State(Type, instance, attribute)                                 \
    (((Type##_Component *)instance)->state.attribute)

#define React_Props(Type, instance, attribute)                                 \
    React_Instance_Props(Type, instance)->attribute

#define React_Header(Type)                                                       \
    typedef struct {                                                             \
        Component      instance;                                                 \
        Type##_props_t props;                                                    \
        Type##_state_t state;                                                    \
    } Type##_Component;                                                          \
    typedef Type##_Component Type##_t;                                           \
    void                     Type##_willMount(void *instance, void *next_props); \
    bool Type##_shouldUpdate(void *instance, void *next_props);                  \
    void Type##_willUpdate(void *instance, void *next_props);                    \
    void Type##_release(void *instance);                                         \
    void Type##_didMount(void *instance);                                        \
    void Type##_didUnmount(void *instance);                                      \
    void Type##_didUpdate(void *instance)

#define React_Self(Type, instance) Type##_Component *self = instance

#if __GNUC__
    #define React_LifeCycle_Header(Type, stage)                                \
        static inline void Type##_inline_##stage(                              \
            Component *self, Type##_props_t *props, Type##_state_t *state)
#else
    #define React_LifeCycle_Header(Type, stage)                                \
        inline void Type##_inline_##stage(                                     \
            Component *self, Type##_props_t *props, Type##_state_t *state)
#endif

#define React_LifeCycle_Headers(Type, stage) React_LifeCycle_Header(Type, stage)

#ifdef REACT_PROFILER
    #define React_Profiler_Count(func)                                             \
        if (current_scope) {                                                   \
            current_scope->func += 1;                                 \
        } else {                                                               \
            calls.func += 1;                                               \
        }
    #define React_Profiler_Tick(component, stage)                                     \
        clock_t begin, end, passed;                                            \
        begin         = clock();                                               \
        current_scope = &component->instance.calls.stage
    #define React_Profiler_Tock(component, stage)                              \
        end    = clock();                                                      \
        passed = end - begin;                                                  \
        cpu_total += passed;                                                   \
        component->instance.cpu.stage += passed;                               \
        component->instance.counter.stage++;                                   \
        current_scope = NULL

#else
    #define React_Profiler_Tick(self, stage)                                          \
        {                                                                      \
        }
    #define React_Profiler_Tock(component, stage)                              \
        {                                                                      \
        }
#endif

#define React_LifeCycle(Type, stage)                                           \
    React_LifeCycle_Headers(Type, stage);                                      \
    void Type##_##stage(void *instance)                                        \
    {                                                                          \
        React_Self(Type, instance);                                            \
        hw_isr_disable();                                                      \
        React_Profiler_Tick(self, stage);                                             \
        Type##_inline_##stage(&self->instance, &self->props, &self->state);    \
        React_Profiler_Tock(self, stage);                                      \
        hw_isr_enable();                                                       \
    }                                                                          \
    React_LifeCycle_Header(Type, stage)

#define React_SelfNext(Type, instance)                                         \
    Type##_props_t *next_props = next_props_ptr

#if __GNUC__
    #define React_UpdateCycle_Header(Type, stage, returnType)                  \
        static inline returnType Type##_inline_##stage(                        \
            Component *self, Type##_props_t *props, Type##_state_t *state,     \
            Type##_props_t *next_props)
#else
    #define React_UpdateCycle_Header(Type, stage, returnType)                  \
        inline returnType Type##_inline_##stage(                               \
            Component *self, Type##_props_t *props, Type##_state_t *state,     \
            Type##_props_t *next_props)
#endif

#define React_UpdateCycle_Headers(Type, stage, returnType)                     \
    React_UpdateCycle_Header(Type, stage, returnType)

#define React_UpdateCycle(Type, stage, returnType)                             \
    React_UpdateCycle_Headers(Type, stage, returnType);                        \
    returnType Type##_##stage(void *instance, void *next_props_ptr)            \
    {                                                                          \
        React_Self(Type, instance);                                            \
        if (!next_props_ptr)                                                   \
            next_props_ptr = &self->props;                                     \
        React_SelfNext(Type, instance);                                        \
        hw_isr_disable();                                                      \
        React_Profiler_Tick(self, stage);                                             \
        returnType result = Type##_inline_##stage(                             \
            &self->instance, &self->props, &self->state, next_props);          \
        React_Profiler_Tock(self, stage);                                      \
        hw_isr_enable();                                                       \
        return result;                                                         \
    }                                                                          \
    React_UpdateCycle_Header(Type, stage, returnType)

#define willMount(Type)                                                        \
    React_UpdateCycle_Headers(Type, willMount, void);                          \
    void Type##_willMount(void *instance, void *next_props_ptr)                \
    {                                                                          \
        React_Self(Type, instance);                                            \
        if (!next_props_ptr)                                                   \
            next_props_ptr = &self->props;                                     \
        React_SelfNext(Type, instance);                                        \
        if (&self->props != next_props)                                        \
            self->props = *next_props;                                         \
        hw_isr_disable();                                                      \
        React_Profiler_Tick(self, willMount);                                             \
        Type##_inline_willMount(&self->instance, &self->props, &self->state,   \
                                next_props);                                   \
        React_Profiler_Tock(self, willMount);                                  \
        hw_isr_enable();                                                       \
    }                                                                          \
    React_UpdateCycle_Header(Type, willMount, void)

#define willUpdate(Type)                                                       \
    React_UpdateCycle_Headers(Type, willUpdate, void);                         \
    void Type##_willUpdate(void *instance, void *next_props_ptr)               \
    {                                                                          \
        React_Self(Type, instance);                                            \
        if (!next_props_ptr)                                                   \
            next_props_ptr = &self->props;                                     \
        React_SelfNext(Type, instance);                                        \
        hw_isr_disable();                                                      \
        React_Profiler_Tick(self, willUpdate);                                             \
        Type##_inline_willUpdate(&self->instance, &self->props, &self->state,  \
                                 next_props);                                  \
        React_Profiler_Tock(self, willUpdate);                                 \
        hw_isr_enable();                                                       \
        if (&self->props != next_props)                                        \
            self->props = *next_props;                                         \
    }                                                                          \
    React_UpdateCycle_Header(Type, willUpdate, void)

#define release(Type)      React_LifeCycle(Type, release)
#define shouldUpdate(Type) React_UpdateCycle(Type, shouldUpdate, bool)
#define didMount(Type)     React_LifeCycle(Type, didMount)
#define didUpdate(Type)    React_LifeCycle(Type, didUpdate)
#define didUnmount(Type)   React_LifeCycle(Type, didUnmount)


#define React_Define_WithProps(Type, instance_name, instance_props)            \
    Type##_Component instance_name = {                                         \
        .instance = React_Define_Component(Type, instance_name),               \
        .props    = instance_props,                                            \
        .state    = {0},                                                       \
    }

#define React_Define_WithState(Type, instance_name, instance_state)            \
    Type##_Component instance_name = {                                         \
        .instance = React_Define_Component(Type, instance_name),               \
        .props    = {0},                                                       \
        .state    = instance_state,                                            \
    }

#ifdef REACT_PROFILER
    #define React_Define_Component(Type, instance_name)                        \
        {                                                                      \
            .stage = REACT_STAGE_DEFINED, .WillMount = Type##_willMount,       \
            .ShouldUpdate = Type##_shouldUpdate,                               \
            .WillUpdate = Type##_willUpdate, .Release = Type##_release,        \
            .DidMount = Type##_didMount, .DidUpdate = Type##_didUpdate,        \
            .name = #Type " / " #instance_name                                 \
        }
#else
    #define React_Define_Component(Type, name)                                 \
        {                                                                      \
            .stage = REACT_STAGE_DEFINED, .WillMount = Type##_willMount,       \
            .ShouldUpdate = Type##_shouldUpdate,                               \
            .WillUpdate = Type##_willUpdate, .Release = Type##_release,        \
            .DidMount = Type##_didMount, .DidUpdate = Type##_didUpdate,        \
        }
#endif

#define React_Define(Type, name) React_Define_WithProps(Type, name, {0})

#define React_Load(Type, instance)                                             \
    Type##_props_t *Type##_props = (instance)->props;                          \
    Type##_state_t *Type##_state = (instance)->state;

#define React_Init(Type, name, props)                                          \
    React_Define_WithProps(Type, name, props);                                 \
    React_Mount(&name)

#define _(...) __VA_ARGS__

#define apply(Type, name, propsValue)                                          \
    {                                                                          \
        if (name.instance.stage == REACT_STAGE_RELEASED                        \
            || name.instance.stage == REACT_STAGE_DEFINED) {                   \
            React_Profiler_Tick((&name), nextProps);                                      \
            Type##_props_t next_props = propsValue;                            \
            React_Profiler_Tock((&name), nextProps);                           \
            Stage_Component(&name.instance, &next_props);                      \
        } else {                                                               \
            Stage_Component(&name.instance, 0);                                \
        }                                                                      \
    }
#define react(Type, name, propsValue)                                          \
    {                                                                          \
        Type##_props_t next_props = propsValue;                                \
        React_Component(&name.instance, &next_props);                          \
    }

#define React_Local(Type, ref)                                                 \
    do {                                                                       \
        Component local                  = Type(&props->ref, &state->ref);     \
        local.stage                      = state->ref##_stage;                 \
        Type##_state_t  local_state      = state->ref;                         \
        Type##_props_t  local_props      = props->ref;                         \
        Type##_props_t *local_next_props = &next_props->ref;                   \
        React(Type)(*local_next_props) to(local);                              \
        state->ref = local_state;                                              \
        props->ref = local_props;                                              \
    } while (0);

#define define(Type, instance_name, instance_props, instance_state)            \
    Type##_Component instance_name = {                                         \
        .instance = React_Define_Component(Type, instance_name),               \
        .props    = instance_props,                                            \
        .state    = instance_state,                                            \
    }

#define use_(x)  Stage_Component(&(x).instance, 0) &&
#define use(...) EVAL(MAP(use_, __VA_ARGS__)) true

#define loop_(x) Stage_Component(&x.instance, 0) &&
#ifdef REACT_PROFILER
    #include <signal.h>
void        sighandler(int sig);
extern bool stop;
    #define loop(...)                                                          \
        signal(SIGINT, sighandler);                                            \
        while (EVAL(MAP(loop_, __VA_ARGS__)) step()                            \
               && !(stop == true && dump_usage()))
#else
    #define loop(...) while (EVAL(MAP(loop_, __VA_ARGS__)) true)
#endif

#define shut(x)                                                                \
    x.instance.stage = REACT_STAGE_UNMOUNTED;                                  \
    Stage_Component(&x.instance, 0);
