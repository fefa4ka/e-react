#include "component.h"

void React_Mount(Component *instance);
void React_Release(Component *instance);

void React_Mount(Component *instance)
{
    instance->stage = released;
    instance->componentWillMount(instance);
    instance->componentRelease(instance);
    instance->componentDidMount(instance);
}

void React_Release(Component *instance)
{
    instance->stage = released;
    instance->componentRelease(instance);
    instance->componentDidUpdate(instance);
}


