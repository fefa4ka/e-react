#pragma once
#include "macros.h"
#include "api.h"

struct ring_buffer {
    unsigned char  *data;
    unsigned int   size;

    unsigned int   read;
    unsigned int   write;
};

unsigned int rb_length(struct ring_buffer *cb);
enum error rb_write(struct ring_buffer *cb, unsigned char data);
enum error rb_write_string(struct ring_buffer *cb, unsigned char *data);
enum error rb_read(struct ring_buffer *cb, unsigned char *data);
