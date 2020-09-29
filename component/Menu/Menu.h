#pragma once 

#include <component.h>

struct menu_command {
    unsigned char         *command;
    void                  (*callback)(void *args);
    void                  *args;

    struct menu_command   *menu;
};

typedef struct
{
    struct menu_command   *menu;
    unsigned char  *command;
    void           (*onCommand)(void *instance);
    void           (*onSelect)(void *instance);
    void           (*onLookup)(void *instance);
    void           (*onWillRun)(void *instance);
} Menu_blockProps;

typedef struct {
    struct menu_command   *previous_menu;
    struct menu_command   *current_menu;
    unsigned char         command[32];
} Menu_blockState;


React_Header(Menu);
#define Menu(instance) component(Menu, instance)