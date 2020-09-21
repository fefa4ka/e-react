#pragma once 

#include <component.h>

typedef struct menu_command_t {
    struct menu_command_t 
                   *menu;
    unsigned char  *command;
    void           (*callback)(void *args);
    void           *args;
} menu_command_t;

typedef struct
{
    menu_command_t *menu;
    unsigned char  *command;
    void           (*onCommand)(void *instance);
    void           (*onSelect)(void *instance);
    void           (*onLookup)(void *instance);
    void           (*onWillRun)(void *instance);
} Menu_blockProps;

typedef struct {
    menu_command_t *previous_menu;
    menu_command_t *current_menu;
    unsigned char  command[32];
} Menu_blockState;


React_Header(Menu);
