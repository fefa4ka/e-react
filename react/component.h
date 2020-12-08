#pragma once

#include <hal.h>
#include <stdbool.h>

typedef struct
{
    enum
    {
        defined,
        prepared,
        released
    } stage;

    void *props;
    void *state;

    void (*componentWillMount) (void *instance);

    bool (*componentShouldUpdate) (void *instance, void *nextProps);
    void (*componentWillUpdate) (void *instance, void *nextProps);

    void (*componentRelease) (void *instance);

    void (*componentDidMount) (void *instance);
    void (*componentDidUpdate) (void *instance);
    void (*componentDidUnmount) (void *instance);
} Component;


void React_Mount (Component *instance);
void React_Release (Component *instance);

// Component Creation
#define React_Header(name)                                                    \
    void name##_block (Component *instance, name##_blockProps *props,         \
                       name##_blockState *state);                             \
    React_Cycle_Header (name)

#define React_Self(Type, instance)                                            \
    Component *        self  = instance;                                      \
    Type##_blockProps *props = (Type##_blockProps *)self->props;              \
    Type##_blockState *state = (Type##_blockState *)self->state

#if __GNUC__
#define React_LifeCycle_Header(Type, stage)                                   \
    extern inline void Type##_##stage (Component *self, Type##_blockProps *props,    \
                                Type##_blockState *state)
#else
#define React_LifeCycle_Header(Type, stage)                                   \
    inline void Type##_##stage (Component *self, Type##_blockProps *props,    \
                                Type##_blockState *state)
#endif

#define React_LifeCycle_Headers(Type, stage)                                   \
    React_LifeCycle_Header(Type, stage);                                   \
    void Type##_generic_##stage (void *instance)                                        \

#define React_LifeCycle(Type, stage)                                          \
    void Type##_generic_##stage (void *instance)                                        \
    {                                                                         \
        React_Self (Type, instance);                                          \
        Type##_##stage (self, props, state);                                  \
    }                                                                         \
    React_LifeCycle_Header(Type, stage)

#define React_SelfNext(Type, instance)                                        \
    Type##_blockProps *nextProps = (Type##_blockProps *)nextProps_p;          \


#if __GNUC__
#define React_UpdateCycle_Header(Type, stage, returnType)                     \
    extern inline returnType Type##_##stage (                                        \
        Component *self, Type##_blockProps *props, Type##_blockState *state,  \
        Type##_blockProps *nextProps)
#else
#define React_UpdateCycle_Header(Type, stage, returnType)                     \
    inline returnType Type##_##stage (                                        \
        Component *self, Type##_blockProps *props, Type##_blockState *state,  \
        Type##_blockProps *nextProps)
#endif

#define React_UpdateCycle_Headers(Type, stage, returnType)                     \
    React_UpdateCycle_Header(Type, stage, returnType);                     \
    returnType Type##_generic_##stage (void *instance, void *nextProps)   \
    
#define React_UpdateCycle(Type, stage, returnType)                            \
    returnType Type##_generic_##stage (void *instance, void *nextProps_p)   \
    {                                                                         \
        React_Self (Type, instance);                                          \
        React_SelfNext (Type, instance);                                      \
        return Type##_##stage (self, props, state, nextProps);     \
    }                                                                         \
    React_UpdateCycle_Header(Type, stage, returnType)

#define willMount(Type)    React_LifeCycle (Type, willMount)
#define release(Type)      React_LifeCycle (Type, release)
#define shouldUpdate(Type) React_UpdateCycle (Type, shouldUpdate, bool)
#define willUpdate(Type)   React_UpdateCycle (Type, willUpdate, void)
#define didMount(Type)     React_LifeCycle (Type, didMount)
#define didUnmount(Type)   React_LifeCycle (Type, didUnmount)
#define didUpdate(Type)    React_LifeCycle (Type, didUpdate)

#define React_Cycle_Header(Type)                                              \
    React_LifeCycle_Headers (Type, willMount);                                 \
    React_LifeCycle_Headers (Type, release);                                   \
    React_UpdateCycle_Headers (Type, shouldUpdate, bool);                      \
    React_UpdateCycle_Headers (Type, willUpdate, void);                        \
    React_LifeCycle_Headers (Type, didMount);                                  \
    React_LifeCycle_Headers (Type, didUnmount);                                \
    React_LifeCycle_Headers (Type, didUpdate)

#define React_Constructor(Type)                                               \
    void Type##_block (Component *instance, Type##_blockProps *props,         \
                       Type##_blockState *state)                              \
    {                                                                         \
        Component Type = { .stage                 = defined,                  \
                           .props                 = props,                    \
                           .state                 = state,                    \
                           .componentWillMount    = Type##_generic_willMount,                \
                           .componentShouldUpdate = Type##_generic_shouldUpdate,             \
                           .componentWillUpdate   = Type##_generic_willUpdate,               \
                           .componentRelease      = Type##_generic_release,                  \
                           .componentDidMount     = Type##_generic_didMount,                 \
                           .componentDidUpdate    = Type##_generic_didUpdate,                \
                           .componentDidUnmount   = Type##_generic_didUnmount };               \
        *instance      = Type;                                                \
    }


// Component Usage

#define React_Define_WithProps(Type, instanceName, instanceProps)                             \
    Type##_blockProps instanceName##Props = instanceProps;                                    \
    Type##_blockState instanceName##State = { 0 };                                    \
    Component instanceName = { .stage                 = defined,                  \
                       .props                 = &instanceName##Props,                    \
                       .state                 = &instanceName##State,                    \
                           .componentWillMount    = Type##_generic_willMount,                \
                           .componentShouldUpdate = Type##_generic_shouldUpdate,             \
                           .componentWillUpdate   = Type##_generic_willUpdate,               \
                           .componentRelease      = Type##_generic_release,                  \
                           .componentDidMount     = Type##_generic_didMount,                 \
                           .componentDidUpdate    = Type##_generic_didUpdate,                \
                           .componentDidUnmount   = Type##_generic_didUnmount                \
    }

#define React_Define(Type, name) React_Define_WithProps (Type, name, { 0 })
#define Define                   React_Define
#define react_define             React_Define
#define component                React_Define

#define React_Load(Type, instance)                                            \
    Type##_blockProps *Type##Props = (instance)->props;                       \
    Type##_blockState *Type##State = (instance)->state;

#define React_Init(Type, name, props)                                         \
    React_Define_WithProps (Type, name, props);                               \
    React_Mount (&name)

#define Mount React_Init

#define React_Idle while (true)

#define _(...) __VA_ARGS__
#define React(Type, name, propsValue)                                              \
    {                                                                         \
        Type##_blockProps nextProps = propsValue;                                  \
        if (name.stage == released                                            \
            && Type##_shouldUpdate (&name, &name##Props, &name##State,          \
                                    &nextProps)) {              \
            name.stage = prepared;                                            \
            Type##_willUpdate (&name, &name##Props, &name##State, &nextProps);                                 \
            name##Props = nextProps;                                          \
        } else if (name.stage == prepared) {                                  \
            name.stage = released; \
            Type##_release(&name, &name##Props, &name##State);\
            Type##_didUpdate(&name, &name##Props, &name##State); \
        } else if (name.stage == defined) {                                   \
            name##Props = nextProps;                                          \
            name.stage = released; \
            Type##_willMount(&name, &name##Props, &name##State);\
            Type##_release(&name, &name##Props, &name##State); \
            Type##_didMount(&name, &name##Props, &name##State); \
        }                                                                     \
    }

#define react React
#define loop  for (;;)


#define React_Local(Type, ref)                                                \
    do {                                                                      \
        Component local                   = Type (&props->ref, &state->ref);  \
        local.stage                       = state->ref##_stage;               \
        Type##_blockState  localState     = state->ref;                       \
        Type##_blockProps  localProps     = props->ref;                       \
        Type##_blockProps *localNextProps = &nextProps->ref;                  \
        React (Type) (*localNextProps) to (local);                            \
        state->ref = localState;                                              \
        props->ref = localProps;                                              \
    } while (0);
