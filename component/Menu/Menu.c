
#include "Menu.h"
#include <string.h>

willMount(Menu) {}

shouldUpdate(Menu)
{
    /* Command not found */
    if (*props->command && strcmp(props->command, state->command) != 0) {
        return true;
    }

    return false;
}

willUpdate(Menu)
{
    struct menu_command *command
        = state->current_menu && state->current_menu->menu
              ? state->current_menu->menu
              : props->menu;

    strcpy(state->command, props->command);
    while (command->command) {
        if (memcmp(command->command, state->command, strlen(command->command))
            == 0) {
            state->previous_menu = state->current_menu;
            state->current_menu  = command;
            return;
        }
        command++;
    }

    if (props->onCommandNotFound)
        props->onCommandNotFound(self);

    state->current_menu = NULL;
}

release(Menu)
{
    struct menu_command *command = state->current_menu;
    if (command) {
        command->callback(command->args);
        if (props->onCommand)
            props->onCommand(self);
    }
}

didMount(Menu) {}
didUpdate(Menu) { *state->command = NULL; }
