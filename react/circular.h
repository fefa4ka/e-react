#ifndef circular_h 
#define circular_h 
#include "common.h"

struct callback {
    void (*method)(void *trigger, void *argument);
    void *argument;
};

struct ring_buffer {
    unsigned char  *data;
    unsigned int   size;

    unsigned int   read;
    unsigned int   write;
};

unsigned int rb_length(struct ring_buffer *cb);
enum eError rb_write(struct ring_buffer *cb, unsigned char data);
enum eError rb_write_string(struct ring_buffer *cb, unsigned char *data);
enum eError rb_read(struct ring_buffer *cb, unsigned char *data);

#endif

