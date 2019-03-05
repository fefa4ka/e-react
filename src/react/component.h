
#ifndef void_h
#define void_h

#include "../macros/types.h"
#include "../macros/log.h"

typedef struct {
    enum
    {
        defined,
        prepared,
        released
    } stage;

    void  * props;
    void  * state;

    void (* componentWillMount)(void *instance);

    bool (* componentShouldUpdate)(void *instance, void *nextProps, void *nextState);
    void (* componentWillUpdate)(void *instance, void *nextProps, void *nextState);

    void (* componentRelease)(void *instance);

    void (* componentDidMount)(void *instance);
    void (* componentDidUpdate)(void *instance);
    void (* componentDidUnmount)(void *instance);
} Component;



// Component Creation

#define React_Self(Type, instance)                   \
    Component *self = (Component *)instance;         \
    Type##Props *props = (Type##Props *)self->props; \
    Type##State *state = (Type##State *)self->state  

#define React_LifeCycle(Type, stage)                                                  \
    static void Type##stage(Component *self, Type##Props *props, Type##State *state); \
    static void stage(void *instance)                                                 \
    {                                                                                 \
        React_Self(Type, instance);                                                   \
        Type##stage(self, props, state);                                              \
    }                                                                                 \
    static void Type##stage(Component *self, Type##Props *props, Type##State *state)

#define React_SelfNext(Type, instance)                   \
    Type##Props *nextProps = (Type##Props *)nextProps_p; \
    Type##State *nextState = (Type##State *)nextState_p  

#define React_UpdateCycle(Type, stage, returnType)                                                                                                  \
    static returnType Type##stage(Component *self, Type##Props *props, Type##State *state, Type##Props *nextProps_p, Type##State *nextState); \
    static returnType stage(void *instance, void *nextProps_p, void *nextState_p)                                                             \
    {                                                                                                                                   \
        React_Self(Type, instance);                                                                                                     \
        React_SelfNext(Type, instance);                                                                                                 \
        return Type##stage(self, props, state, nextProps, nextState);                                                                          \
    }                                                                                                                                   \
    static returnType Type##stage(Component *self, Type##Props *props, Type##State *state, Type##Props *nextProps, Type##State *nextState)

#define willMount(Type) React_LifeCycle(Type, willMount)
#define release(Type) React_LifeCycle(Type, release)
#define shouldUpdate(Type) React_UpdateCycle(Type, shouldUpdate, bool)
#define willUpdate(Type) React_UpdateCycle(Type, willUpdate, void)
#define didMount(Type) React_LifeCycle(Type, didMount)
#define didUnmount(Type) React_LifeCycle(Type, didUnmount)
#define didUpdate(Type) React_LifeCycle(Type, didUpdate)

#define React_Constructor(Type)                            \
    Component Type(Type##Props *props, Type##State *state) \
    {                                                      \
        Component instance = {                             \
            .stage = defined,                              \
            .props = props,                                \
            .state = state,                                \
            .componentWillMount = willMount,               \
            .componentShouldUpdate = shouldUpdate,         \
            .componentWillUpdate = willUpdate,             \
            .componentRelease = release,                   \
            .componentDidMount = didMount,                 \
            .componentDidUpdate = didUpdate,               \
            .componentDidUnmount = didUnmount};            \
        return instance;                                   \
    };



// Component Usage

#define React_Define_WithProps(Type, name, props) \
    Type##Props name##Props = (Type##Props)props; \
    Type##State name##State = (Type##State){0};   \
    Component name = Type(&name##Props, &name##State)

#define React_Define(Type, name) React_Define_WithProps(Type, name, {0})

#define React_Init(Type, name, props) \
    React_Define_WithProps(Type, name, props);  \
    React_Mount(&name)

#define React_Idle while(true)

#define React(Type) \
    do { \
        Type##Props nextProps =

#define to(name)                                                                       \
    ;                                                                                         \
    if (name.stage == defined)                                                                \
    {                                                                                         \
        name##Props = nextProps;                                                              \
        React_Mount(&name);                                                                   \
    }                                                                                         \
    if (name.stage == released && name.componentShouldUpdate(&name, &nextProps, &name##State)) \
    {                                                                                         \
        name.componentWillUpdate(&name, &nextProps, &name##State);                            \
        name##Props = nextProps;                                                              \
        name.stage = prepared;                                                                \
    }                                                                                         \
    if (name.stage == prepared)                                                               \
    {                                                                                         \
        React_Release(&name);                                                                 \
    }                                                                                         \
    }                                                                                         \
    while (0)                                                                                 \
        ;

#endif