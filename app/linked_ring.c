#include "unit.h"


#include <linked_ring.h>
#define BUFFER_SIZE 32

int main(void)
{
    void *letter, *number, *pointer, *null;

    struct lr_cell cells[BUFFER_SIZE] = {0};
    struct linked_ring buffer = { .cells = cells, .size = BUFFER_SIZE };

    char *hello = "Hello world";

    void *r_ptr;
    unsigned char r_chr;
    uint16_t r_nbr;

    printf("Owner letter: %lld\n", lr_owner(&cells));
    printf("Owner number: %lld\n", lr_owner(&number));
    printf("Owner pointers: %lld\n", lr_owner(&pointer));
    printf("Owner null: %lld\n", lr_owner(&null));

    /* Add pointer to buffer */
    lr_write(&buffer, hello, lr_owner(&pointer) & 0xF0);

    lr_write(&buffer, 'a', lr_owner(&letter));

    lr_read(&buffer, &r_ptr, lr_owner(&null));

    lr_write(&buffer, 1, lr_owner(&number));

    lr_read(&buffer, &r_ptr, lr_owner(&null));

    lr_write(&buffer, 'b', lr_owner(&letter));
    lr_write(&buffer, 65535, lr_owner(&number));

    lr_read(&buffer, &r_ptr, lr_owner(&null));

    lr_write(&buffer, '!', lr_owner(&null));
    lr_write(&buffer, '@', lr_owner(&null));
    lr_write(&buffer, '$', lr_owner(&null));

    lr_read(&buffer, &r_chr, lr_owner(&letter));
    printf("Pop a: %c\n", r_chr);

    lr_read(&buffer, &r_chr, lr_owner(&letter));
    printf("Pop b: %c\n", r_chr);

    lr_write(&buffer, '%', lr_owner(&null));

    lr_read(&buffer, &r_nbr, lr_owner(&number));
    printf("Pop 1: %d\n", r_nbr);

    lr_read(&buffer, &r_nbr, lr_owner(&number));
    printf("Pop 65535: %d\n", r_nbr);

    lr_write(&buffer, '*', lr_owner(&null));

    lr_read(&buffer, &r_ptr, lr_owner(&pointer) & 0xF0);

    printf("Pop %p: %p = %s\n", hello, r_ptr, (char *)r_ptr);
//        printf("%d Data available for %lld\n", lr_length_owned(&buffer, lr_owner(&buffer)), lr_owner(&buffer));
}
