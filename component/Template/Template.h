#pragma once 

#include <stdbool.h>
#include <component.h>

typedef struct
{
    pin_t      pin;
    void       (*callback)(Component *instance);
} Template_blockProps;

typedef struct {
    bool level;
} Template_blockState;

React_Header(Template);
#define Template(instance) component(Template, instance)
