#pragma once
#include <stdint.h>
#include "macros.h"

#define lr_owner_t uintptr_t
#define lr_data_t unsigned char
#define lr_owner (uintptr_t)
struct lr_cell {
    lr_data_t   data;
    lr_owner_t      owner;
    struct lr_cell *next;
};

struct linked_ring {
    struct lr_cell *cells;
    unsigned int    size;

    lr_owner_t      owners;
    struct lr_cell *read;
    struct lr_cell *write;
};

unsigned int lr_length(struct linked_ring *lr);
enum error   lr_write(struct linked_ring *lr, lr_data_t data, lr_owner_t owner);
enum error   lr_write_string(struct linked_ring *lr, lr_data_t *data, lr_owner_t owner);
enum error   lr_read(struct linked_ring *lr, lr_data_t *data, lr_owner_t owner);
enum error   lr_read_cells(struct linked_ring *lr, unsigned int number,
                           lr_data_t *data, lr_owner_t owner);
