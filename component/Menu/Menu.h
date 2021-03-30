#pragma once 

#include <component.h>

#define Menu(instance) React_Define(Menu, instance)

struct menu_command {
    unsigned char         *command;
    void                  (*callback)(Component *instance);
    void                  *args;

    struct menu_command   *menu;
};

typedef struct
{
    struct menu_command   *menu;
    unsigned char  *command;
    void           (*onCommand)(Component *instance);
    void           (*onCommandNotFound)(Component *instance);
} Menu_props_t;

typedef struct {
    struct menu_command   *previous_menu;
    struct menu_command   *current_menu;
    unsigned char         command[32];
} Menu_state_t;


React_Header(Menu);
