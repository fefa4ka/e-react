#pragma once 

#include <component.h>

#define Template(instance, props) define(Template, instance, _(props), {0})

typedef struct
{
    pin_t      pin;
    void       (*callback)(Component *instance);
} Template_props_t;

typedef struct {
    bool level;
} Template_state_t;

React_Header(Template);
