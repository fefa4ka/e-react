#pragma once

#include "macros.h"

#define lr_data_t    uintptr_t
#define lr_data(ptr) (uintptr_t) ptr

#define lr_owner_t uintptr_t
/* Add for bit trimming */
#define lr_owner(ptr) ((uintptr_t)ptr + (uintptr_t)ptr)

struct lr_cell {
    lr_data_t       data;
    lr_owner_t      owner;
    struct lr_cell *next;
};

struct linked_ring {
    struct lr_cell *cells;
    unsigned int    size;

    lr_owner_t owners;

    struct lr_cell *read;
    struct lr_cell *write;
    struct lr_cell *tail;
};

uint16_t   lr_length(struct linked_ring *lr);
bool       lr_exists(struct linked_ring *lr, lr_owner_t owner);
uint16_t   lr_length_owned(struct linked_ring *lr, lr_owner_t owner);
enum error lr_write(struct linked_ring *lr, lr_data_t data, lr_owner_t owner);
enum error lr_write_string(struct linked_ring *lr, lr_data_t *data,
                           lr_owner_t owner);
enum error lr_read(struct linked_ring *lr, lr_data_t *data, lr_owner_t owner);
enum error lr_read_cells(struct linked_ring *lr, unsigned int number,
                         lr_data_t *data, lr_owner_t owner);
#ifdef REACT_PROFILER
enum error lr_dump(struct linked_ring *lr);
#endif
