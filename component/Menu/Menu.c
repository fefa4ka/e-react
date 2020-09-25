
#include "Menu.h"
#include <string.h>

willMount(Menu) {
}

shouldUpdate(Menu) {
    /* Command not found */
    if(strcmp(props->command, state->command) != 0) {
        if(props->onCommand) props->onCommand(self);
        return true;
    }

    return false;
}

willUpdate(Menu) {
    struct menu_command *command = state->current_menu && state->current_menu->menu
        ? state->current_menu->menu
        : props->menu;

    strcpy(state->command, props->command);
    while(command) {
        if(strcmp(command->command, props->command) == 0) {
            if(props->onSelect) props->onSelect(self);
            state->previous_menu = state->current_menu;
            state->current_menu = command;
            return;
        }
        command++;
    }

    state->current_menu = NULL;
}

release(Menu) {
    struct menu_command *command = state->current_menu;
    if(command) {
        command->callback(command->args);
    }
}

didMount(Menu) {
}

didUnmount(Menu) { }
didUpdate(Menu) {
}

React_Constructor(Menu);
