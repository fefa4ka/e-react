
#ifndef void_h
#define void_h

#include <stdbool.h>

typedef struct {
    enum {
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


void React_Mount(Component *instance);
void React_Release(Component *instance);

// Component Creation
#define React_Header(name) Component name##_block(name##_blockProps *props, name##_blockState *state)

#define React_Self(Type, instance)                   \
    Component *self = (Component *)instance;         \
    Type##_blockProps *props = (Type##_blockProps *)self->props; \
    Type##_blockState *state = (Type##_blockState *)self->state  

#define React_LifeCycle(Type, stage)                                                  \
    static void Type##stage(Component *self, Type##_blockProps *props, Type##_blockState *state); \
    static void stage(void *instance)                                                 \
    {                                                                                 \
        React_Self(Type, instance);                                                   \
        Type##stage(self, props, state);                                              \
    }                                                                                 \
    static inline void Type##stage(Component *self, Type##_blockProps *props, Type##_blockState *state)

#define React_SelfNext(Type, instance)                   \
    Type##_blockProps *nextProps = (Type##_blockProps *)nextProps_p; \
    Type##_blockState *nextState = (Type##_blockState *)nextState_p  

#define React_UpdateCycle(Type, stage, returnType)                                                                                                  \
    static returnType Type##stage(Component *self, Type##_blockProps *props, Type##_blockState *state, Type##_blockProps *nextProps_p, Type##_blockState *nextState); \
    static returnType stage(void *instance, void *nextProps_p, void *nextState_p)                                                             \
    {                                                                                                                                   \
        React_Self(Type, instance);                                                                                                     \
        React_SelfNext(Type, instance);                                                                                                 \
        return Type##stage(self, props, state, nextProps, nextState);                                                                          \
    }                                                                                                                                   \
    static inline returnType Type##stage(Component *self, Type##_blockProps *props, Type##_blockState *state, Type##_blockProps *nextProps, Type##_blockState *nextState)

#define willMount(Type) React_LifeCycle(Type, willMount)
#define release(Type) React_LifeCycle(Type, release)
#define shouldUpdate(Type) React_UpdateCycle(Type, shouldUpdate, bool)
#define willUpdate(Type) React_UpdateCycle(Type, willUpdate, void)
#define didMount(Type) React_LifeCycle(Type, didMount)
#define didUnmount(Type) React_LifeCycle(Type, didUnmount)
#define didUpdate(Type) React_LifeCycle(Type, didUpdate)

#define React_Constructor(Type)                            \
    Component Type##_block(Type##_blockProps *props, Type##_blockState *state) \
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
    Type##_blockProps name##Props = (Type##_blockProps)props; \
    Type##_blockState name##State = (Type##_blockState){0};   \
    Component name = Type##_block(&name##Props, &name##State)

#define React_Define(Type, name) React_Define_WithProps(Type, name, {0})
#define Define React_Define
#define react_define React_Define

#define React_Load(Type, instance) \
    Type##_blockProps *Type##_blockProps = (instance)->props; \
    Type##_blockState *Type##_blockState = (instance)->state;

#define React_Init(Type, name, props) \
    React_Define_WithProps(Type, name, props);  \
    React_Mount(&name)

#define Mount React_Init

#define React_Idle while(true)

#define React(Type) \
    do { \
        Type##_blockProps nextProps =

#define react React

// Difficult to realize in function. 
#define to(name)                                                                       \
    ;                                                                                         \
    if (name.stage == released && name.componentShouldUpdate(&name, &nextProps, &name##State)) \
    {                                                                                         \
        name.stage = prepared;                                                                \
        name.componentWillUpdate(&name, &nextProps, &name##State);                            \
        name##Props = nextProps;                                                              \
    }                                                                                         \
    else if (name.stage == prepared)                                                               \
    {                                                                                         \
        React_Release(&name);                                                                 \
    }                                                                                         \
    else if (name.stage == defined)                                                                \
    {                                                                                         \
        name##Props = nextProps;                                                              \
        React_Mount(&name);                                                                   \
    }                                                                                         \
    }                                                                                         \
    while (0)                                                                                 \
        ;

#define React_Local(Type, ref) \
    do { \
        Component local = Type(&props->ref, &state->ref); \
        local.stage = state->ref##_stage; \
        Type##_blockState localState = state->ref; \
        Type##_blockProps localProps = props->ref; \
        Type##_blockProps *localNextProps = &nextProps->ref; \
        React(Type) (*localNextProps) to(local); \
        state->ref = localState; \
        props->ref = localProps; \
    } while(0) \
    ;

#endif
