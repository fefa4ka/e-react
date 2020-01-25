#ifndef utils_h
#define utils_h 
#include "common.h"

struct sCircularBuffer {
    unsigned int read;
    unsigned int write;
    unsigned int size;
    unsigned char  *data;
    unsigned char  *owner;
};

unsigned int CBLengthData(struct sCircularBuffer *cb);
enum eError CBWrite(struct sCircularBuffer *cb, unsigned char data);
enum eError CBWriteOwner(struct sCircularBuffer *cb, unsigned char data, unsigned char owner);
enum eError CBRead(struct sCircularBuffer *cb, unsigned char *data);
enum eError CBReadOwner(struct sCircularBuffer *cb, unsigned char owner);

#endif

