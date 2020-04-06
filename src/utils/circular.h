#ifndef circular_h 
#define circular_h 
#include "common.h"

struct sCircularBuffer {
    unsigned int read;
    unsigned int write;
    unsigned int size;
    unsigned char  *data;
    void         **owner;
};

unsigned int CBLengthData(struct sCircularBuffer *cb);
enum eError CBWrite(struct sCircularBuffer *cb, unsigned char data);
enum eError CBWriteOwner(struct sCircularBuffer *cb, unsigned char data, void *owner);
enum eError CBRead(struct sCircularBuffer *cb, unsigned char *data);
enum eError CBReadOwner(struct sCircularBuffer *cb, void *owner);

#endif

