#include "linked_ring.h"

#ifdef REACT_PROFILER
    #include "profiler.h"
#endif

uint16_t lr_length_limited_owned(struct linked_ring *lr, uint16_t limit,
                                 lr_owner_t owner)
{
    uint8_t         length  = 0;
    struct lr_cell *counter = lr->read;


    /* Empty buffer */
    if (lr->read == 0)
        return 0;

    if (owner && (lr->owners & owner) != owner)
        return 0;

    /* Full buffer */
    if (!owner && lr->write == 0)
        return lr->size;

    /* Iterate from read to cell with *next = &lr.write */
    do {
        if (!owner || counter->owner == owner)
            length++;
        counter = counter->next;
        REACT_PROFILER_COUNT(lr_seek);
    } while ((counter->next && counter != lr->write)
             && (!limit || limit == length));

    return length;
}

uint16_t lr_length_owned(struct linked_ring *lr, lr_owner_t owner)
{
    REACT_PROFILER_COUNT_LOG(lr_length_owned, "(%x)=%d", owner, lr_length_limited_owned(lr, 0, owner));
    return lr_length_limited_owned(lr, 0, owner);
}

bool lr_exists(struct linked_ring *lr, lr_owner_t owner)
{
    REACT_PROFILER_COUNT_LOG(lr_exists, "(%x)=%d", owner, lr_length_limited_owned(lr, 1, owner));
    return lr_length_limited_owned(lr, 1, owner);
}

uint16_t lr_length(struct linked_ring *lr)
{
    REACT_PROFILER_COUNT_LOG(lr_length, "()=%d", lr_length_limited_owned(lr, 0, 0));
    return lr_length_limited_owned(lr, 0, 0);
}

enum error lr_write(struct linked_ring *lr, lr_data_t data, lr_owner_t owner)
{
    struct lr_cell *recordable_cell;

    if (!lr->write && lr->read)
        return ERROR_BUFFER_FULL;

    /* If free buffer */
    if (!lr->write)
        lr->write = lr->cells;

    recordable_cell = lr->write;
    recordable_cell->data  = data;
    recordable_cell->owner = owner;

    lr->owners |= owner;

    /* Initialize next pointer, if not set */
    if (!recordable_cell->next) {
        /* next = 0 if it is end of the buffer */
        if (recordable_cell != (lr->cells + lr->size))
            recordable_cell->next = recordable_cell + 1;
        else if (lr->read)
            recordable_cell->next = lr->read;
        /*
        else
            recordable_cell->next = lr->cells;
        */
    }

    if (recordable_cell->next == lr->read)
        /* Last available cell */
        lr->write = 0;
    else
        lr->write = recordable_cell->next;

    if (!lr->read)
        lr->read = recordable_cell;

    REACT_PROFILER_COUNT_LOG(lr_write, "(%x, %x)", data, owner);

    return ERROR_NONE;
}


enum error lr_read(struct linked_ring *lr, lr_data_t *data, lr_owner_t owner)
{
    struct lr_cell *readable_cell = lr->read;
    struct lr_cell *previous_cell = 0;
    struct lr_cell *freed_cell    = 0;
    struct lr_cell *needle        = lr->write ? lr->write : lr->read;

    REACT_PROFILER_COUNT_LOG(lr_read, "(%x)", owner);

    if (owner && (lr->owners & owner) != owner)
        return ERROR_BUFFER_EMPTY;

    /* Flush owners, and set again during buffer reading */
    /* O(n) = buffer_length */
    lr->owners = 0;

    do {
        struct lr_cell *next_cell = readable_cell->next;

        if (!owner || readable_cell->owner == owner) {
            *data = readable_cell->data;
            /* For skipping next match set impossible owner */
            // TODO: max lr_owner_t
            owner = 0xFFFF;

            /* Reassembly linking ring */
            if (previous_cell) {
                /* Link cells between */
                if (readable_cell->next == needle) {
                    lr->write           = readable_cell;
                    previous_cell->next = readable_cell;

                    return ERROR_NONE;
                } else {
                    previous_cell->next = readable_cell->next;
                }
            } else {
                /* If readed last available cell */
                if (readable_cell->next == needle) {
                    lr->read  = 0;
                    lr->write = readable_cell;

                    return ERROR_NONE;
                } else {
                    /* Once case when read pointer changing
                     * If readed first cell */
                    lr->read = readable_cell->next;
                }
            }

            freed_cell = readable_cell;
            if (lr->write)
                /* Add cell on top of the buffer */
                freed_cell->next = lr->write;
            else
                freed_cell->next = lr->read;
        } else {
            /* All cells owners digest */
            lr->owners |= readable_cell->owner;
        }

        /* Cell iteration */
        previous_cell = readable_cell;
        readable_cell = next_cell;
        REACT_PROFILER_COUNT(lr_seek);
    } while (readable_cell->next && readable_cell != needle);

    if (owner != 0xFFFF) {
        return ERROR_BUFFER_EMPTY;
    } else {
        /* Last iteration. Link freed cell as next */
        previous_cell->next = freed_cell;

        lr->write = freed_cell;
    }

    return ERROR_NONE;
}

enum error lr_write_string(struct linked_ring *lr, lr_data_t *data,
                           lr_owner_t owner)
{
    while (*data) {
        if (lr_write(lr, *(data++), owner) == ERROR_BUFFER_FULL)
            return ERROR_BUFFER_FULL;
    };

    return ERROR_NONE;
}

enum error lr_dump(struct linked_ring *lr)
{
    if (lr_length(lr) == 0)
        return ERROR_BUFFER_EMPTY;

    printf("size=%d\n", lr_length(lr));
    struct lr_cell *readable_cell = lr->read;
    do {
        struct lr_cell *next_cell = readable_cell->next;

        printf("%x: %x | ", readable_cell->owner, readable_cell->data);
        readable_cell = next_cell;
    } while (readable_cell->next && readable_cell != lr->write);

    printf("\n");

    return ERROR_NONE;
}
