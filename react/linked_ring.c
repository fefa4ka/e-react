#include "linked_ring.h"

enum error
lr_length_owned(struct linked_ring *lr, lr_owner_t owner) {
    uint8_t length = 0;
    struct lr_cell *counter = lr->read;

    /* Empty buffer */
    if(lr->read == 0) {
        return 0;
    }

    if(owner && (lr->owners & owner) != owner) {
        return 0;
    }

    /* Full buffer */
    if(lr->write == 0) {
        return lr->size;
    }

    /* Iterate from read to cell with *next = &lr.write */
    do {
        if(!owner || counter->owner == owner) {
            length++;
        }
        counter++;
    } while(counter->next);

    return length;
}

unsigned int
lr_length(struct linked_ring *lr) {
    return lr_length_owned(lr, 0);
}


enum error
lr_write(struct linked_ring *lr, lr_data_t data, lr_owner_t owner){
    struct lr_cell *recordable_cell;

    if (lr_length(lr) == lr->size) {
        return ERROR_BUFFER_FULL;
    }

    /* If free buffer */
    if(!lr->write) {
        recordable_cell = lr->cells;
    } else {
        recordable_cell = lr->write;
    }

    recordable_cell->data = data;
    recordable_cell->owner = owner;

    lr->owners |= owner;

    /* Initialize next pointer, if not set */
    if(!recordable_cell->next) {
        /* next = 0 if it is end of the buffer */
        if(recordable_cell != (lr->cells + lr->size)) {
            recordable_cell->next = recordable_cell + 1;
        }
    }

    lr->write = recordable_cell->next;
    if(!lr->read) {
        lr->read = recordable_cell;
    }

    return ERROR_NONE;
}


enum error
lr_read(struct linked_ring *lr, lr_data_t *data, lr_owner_t owner) {
    struct lr_cell *readable_cell = lr->read;
    struct lr_cell *previous_cell = 0;

    if (lr_length(lr) == 0) { return ERROR_BUFFER_EMPTY; }

    if((lr->owners & owner) != owner) {
        return ERROR_BUFFER_EMPTY;
    }

    /* Flush owners, and set again during buffer reading */
    /* O(n) = buffer_length */
    lr->owners = 0;

    do {
        struct lr_cell *next_cell = readable_cell->next;

        if(!owner || readable_cell->owner == owner) {
            *data = readable_cell->data;
            /* For skipping next match set impossible owner */
            // TODO: max lr_owner_t
            owner = 0xFFFF;

            /* Reassembly linking ring */
            if(previous_cell) {
                /* Link cells between */
                previous_cell->next = readable_cell->next;
            } else {
                /* If readed last available cell */
                if(!readable_cell->next->next) {
                    lr->read = 0;

                    return ERROR_NONE;
                } else {
                    /* Once case when read pointer changing
                     * If readed first cell */
                    lr->read = readable_cell->next;
                }
            }

            if(!lr->write) {
                /* If the buffer is full allocate one cell */
                lr->write = readable_cell;
                lr->write->next = 0;
            } else {
                /* Add cell on top of the buffer */
                readable_cell->next = lr->write;
                lr->write = readable_cell;
            }
        } else {
            /* All cells owners digest */
            lr->owners |= readable_cell->owner;
        }

        /* Cell iteration */
        previous_cell = readable_cell;
        readable_cell = next_cell;
    } while(readable_cell->next);

    if(owner != 0xFFFF) {
        return ERROR_BUFFER_EMPTY;
    }

    return ERROR_NONE;
}

enum error
lr_write_string(struct linked_ring *lr, lr_data_t *data, lr_owner_t owner)
{
    while(*data) {
        if(lr_write(lr, *(data++), owner) == ERROR_BUFFER_FULL) {
            return ERROR_BUFFER_FULL;
        }
    };

    return ERROR_NONE;
}
