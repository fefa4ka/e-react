#include "component.h"

void React_Mount (Component *instance);
void React_Release (Component *instance);

void
React_Mount (Component *instance)
{
    instance->stage = released;
    instance->WillMount (instance);
    instance->Release (instance);
    instance->DidMount (instance);
}

void
React_Release (Component *instance)
{
    instance->stage = released;
    instance->Release (instance);
    instance->DidUpdate (instance);
}
