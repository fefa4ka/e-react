#include "component.h"


bool React_Component(Component *instance, void *next_props) {
    if (instance->stage == REACT_STAGE_RELEASED
        && instance->ShouldUpdate(instance, next_props)) {
        instance->stage = REACT_STAGE_PREPARED;
        instance->WillUpdate(instance, next_props);
    } else if (instance->stage == REACT_STAGE_PREPARED) {
        instance->stage = REACT_STAGE_RELEASED;
        instance->Release(instance);
        instance->DidUpdate(instance);
    } else if (instance->stage == REACT_STAGE_DEFINED) {
        instance->WillMount(instance, next_props);
        instance->Release(instance);
        instance->DidMount(instance);
        instance->stage = REACT_STAGE_RELEASED;
    }

    return true;
}

