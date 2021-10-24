#include "component.h"

bool Stage_Component(Component *instance, void *next_props)
{
    if (instance->stage == REACT_STAGE_RELEASED
        && instance->ShouldUpdate(instance, next_props)) {
        instance->stage = REACT_STAGE_PREPARED;
        instance->WillUpdate(instance, next_props);
    } else if (instance->stage == REACT_STAGE_PREPARED) {
        instance->stage = REACT_STAGE_RELEASED;
        instance->Release(instance);
        instance->DidUpdate(instance);
    } else if (instance->stage == REACT_STAGE_DEFINED) {
#ifdef REACT_PROFILER
        hash_write(&scope, hash_component(instance->name), (void **)instance);
#endif
        instance->WillMount(instance, next_props);
        instance->Release(instance);
        instance->DidMount(instance);
        instance->stage = REACT_STAGE_RELEASED;
    } else if (instance->stage == REACT_STAGE_UNMOUNTED) {
        instance->stage = REACT_STAGE_BLOCKED;
        instance->DidUnmount(instance);
    }

    return true;
}

bool React_Component(Component *instance, void *next_props)
{
    if (instance->ShouldUpdate(instance, next_props)) {
        instance->stage = REACT_STAGE_PREPARED;
        instance->WillUpdate(instance, next_props);
        instance->stage = REACT_STAGE_RELEASED;
        instance->Release(instance);
        instance->DidUpdate(instance);
    }

    if (instance->stage == REACT_STAGE_UNMOUNTED) {
        instance->stage = REACT_STAGE_BLOCKED;
        instance->DidUnmount(instance);
    }

    return true;
}
