#include "hash.h"
/*
unsigned int index[8];
struct callback *methods[8];
struct hash_table buffer = {
    .index = index,
    .data = methods,
    .size = 8,
    .used = 0
}
*/

enum error 
hash_read(struct hash_table *hash, unsigned int key, void **ptr) {
    for(unsigned int index = 0; index <= hash->used; index++) {
        if(hash->index[index] == key) {
            *ptr = hash->data[index];

            return ERROR_NONE;
        }
    }
    return ERROR_BUFFER_EMPTY;
}

enum error
hash_pop(struct hash_table *hash, unsigned int key, void **ptr) {
    for(unsigned int index = 0; index <= hash->used; index++) {
        if(hash->index[index] == key) {
            if(ptr)
                *ptr = (void*)hash->data[index];
            hash->data[index] = hash->data[hash->used - 1];
            hash->index[index] = hash->index[hash->used - 1];
            hash->used -= 1;

            return ERROR_NONE;
        }
    }

    return ERROR_BUFFER_EMPTY;
}

enum error
hash_write(struct hash_table *hash, unsigned int key, void *data) {
    for(unsigned int index = 0; index <= hash->used; index++) {
        if(hash->index[index] == key) {
            hash->data[index] = data;

            return ERROR_NONE;
        }
    }
    if(hash->size > hash->used) {
        hash->index[hash->used] = key;
        hash->data[hash->used] = data;
        hash->used += 1;

        return ERROR_NONE;
    } 

    return ERROR_BUFFER_FULL;
}
