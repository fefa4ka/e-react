#include "circular.h"

unsigned int rb_length(struct ring_buffer *cb);
enum error rb_write(struct ring_buffer *cb, unsigned char data);
enum error rb_read(struct ring_buffer *cb, unsigned char *data);
enum error rb_write_string(struct ring_buffer *cb, unsigned char *data) ;


unsigned int 
rb_length(struct ring_buffer *cb) {
/* Slower version */
   //int length = cb->write - cb->read;
   //if (length >= 0) { return length; }
   //return cb->size - cb->write + cb->read;

   return ((cb->write - cb->read) & (cb->size - 1));
}

enum error 
rb_write(struct ring_buffer *cb, unsigned char data){ 
    if (rb_length(cb) == (cb->size - 1)) {
        return ERROR_BUFFER_FULL;
    } 
    
    cb->data[cb->write] = data;

    cb->write = (cb->write + 1) & (cb->size - 1); // must be atomic } The modification
    
    return ERROR_NONE;
}


enum error 
rb_read(struct ring_buffer *cb, unsigned char *data) {
    if (rb_length(cb) == 0) { return ERROR_BUFFER_EMPTY; }
    *data = cb->data[cb->read];
    cb->read = (cb->read + 1) & ( cb->size - 1);

    return ERROR_NONE;
}

enum error 
rb_write_string(struct ring_buffer *cb, unsigned char *data) 
{
    while(*data) {
        if(rb_write(cb, *(data++)) == ERROR_BUFFER_FULL) {
            return ERROR_BUFFER_FULL;
        }
    }; 

    return ERROR_NONE;
}
