#include "circular.h"

unsigned int rb_length(struct ring_buffer_s *cb);
enum eError rb_write(struct ring_buffer_s *cb, unsigned char data);
enum eError rb_read(struct ring_buffer_s *cb, unsigned char *data);
enum eError rb_write_string(struct ring_buffer_s *cb, unsigned char *data) ;

unsigned int rb_length(struct ring_buffer_s *cb) {
/* Slower version */
   //int length = cb->write - cb->read;
   //if (length >= 0) { return length; }
   //return cb->size - cb->write + cb->read;

   return ((cb->write - cb->read) & (cb->size - 1));
}

enum eError rb_write(struct ring_buffer_s *cb, unsigned char data){ 
    if (rb_length(cb) == (cb->size - 1)) {
        return eErrorBufferFull;
    } 
    
    cb->data[cb->write] = data;

    cb->write = (cb->write + 1) & (cb->size - 1); // must be atomic } The modification
    
    return eErrorNone;
}


enum eError rb_read(struct ring_buffer_s *cb, unsigned char *data) {
    if (rb_length(cb) == 0) { return eErrorBufferEmpty; }
    *data = cb->data[cb->read];
    cb->read = (cb->read + 1) & ( cb->size - 1);

    return eErrorNone;
}

enum eError rb_write_string(struct ring_buffer_s *cb, unsigned char *data) 
{
    while(*data) {
        if(rb_write(cb, *(data++)) == eErrorBufferFull) {
            return eErrorBufferFull;
        }
    }; 

    return eErrorNone;
}
