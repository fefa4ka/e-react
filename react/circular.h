#ifndef circular_h 
#define circular_h 
#include "common.h"

struct ring_buffer_s {
    unsigned int read;
    unsigned int write;
    unsigned int size;
    unsigned char  *data;
};

unsigned int rb_length(struct ring_buffer_s *cb);
enum eError rb_write(struct ring_buffer_s *cb, unsigned char data);
enum eError rb_write_string(struct ring_buffer_s *cb, unsigned char *data);
enum eError rb_read(struct ring_buffer_s *cb, unsigned char *data);

#endif

