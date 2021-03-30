#pragma once

#include "macros.h"

struct hash_table {
    unsigned int *index;
    void **data;
    unsigned int size;
    unsigned int used;
};

enum error hash_read(struct hash_table *, unsigned int key, void **ptr);
enum error hash_write(struct hash_table *, unsigned int key, void *data);
enum error hash_pop(struct hash_table *, unsigned int key, void **ptr);


