#include "component.h"

void React_Mount(Component *instance)
{
    instance->componentWillMount(instance);
    instance->componentRelease(instance);
    instance->componentDidMount(instance);
    instance->stage = released;
}

void React_Release(Component *instance)
{
    instance->componentRelease(instance);
    instance->componentDidUpdate(instance);
    instance->stage = released;
}
