#include "unit.h"

#include <linked_ring.h>

#define BUFFER_SIZE 32

int main(void)
{
    struct lr_cell cells[BUFFER_SIZE] = {0};
    struct linked_ring buffer = { .cells = cells, .size = BUFFER_SIZE };
    unsigned char r;

    printf("Owner: %lld\n", lr_owner(&cells));
    printf("Owner: %lld\n", lr_owner(&buffer));

    lr_write(&buffer, 'a', lr_owner(&cells));

    lr_read(&buffer, &r, lr_owner(&buffer));
    lr_write(&buffer, '1', lr_owner(&buffer));
    lr_read(&buffer, &r, lr_owner(&r));
    lr_write(&buffer, 'b', lr_owner(&cells));
    lr_write(&buffer, '2', lr_owner(&buffer));
    lr_read(&buffer, &r, lr_owner(&r));
    lr_read(&buffer, &r, lr_owner(&cells));
    printf("Pop: %c\n", r);
    lr_read(&buffer, &r, lr_owner(&cells));
    printf("Pop: %c\n", r);

    lr_read(&buffer, &r, lr_owner(&buffer));
    printf("Pop: %c\n", r);
    lr_read(&buffer, &r, lr_owner(&buffer));
    printf("Pop: %c\n", r);
    if(lr_read(&buffer, &r, lr_owner(&buffer)) == 0) {
        printf("Pop: %c\n", r);
    }
}
