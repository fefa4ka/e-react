#include "circular.h"

unsigned int CBLengthData(struct sCircularBuffer *cb);
enum eError CBWrite(struct sCircularBuffer *cb, unsigned char data);
enum eError CBWriteOwner(struct sCircularBuffer *cb, unsigned char data, unsigned char owner);
enum eError CBRead(struct sCircularBuffer *cb, unsigned char *data);
enum eError CBReadOwner(struct sCircularBuffer *cb, unsigned char owner);

unsigned int CBLengthData(struct sCircularBuffer *cb) {
/* Slower version */
   int length = cb->write - cb->read;
   if (length >= 0) { return length; }
   return cb->size - cb->write + cb->read;

   //return ((cb->write - cb->read) & (cb->size - 1));
}

enum eError CBWrite(struct sCircularBuffer *cb, unsigned char data){ 
    if (CBLengthData(cb) == (cb->size - 1)) {
        return eErrorBufferFull;
    } 
    
    cb->data[cb->write] = data;

    cb->write++; // = (cb->write + 1) & (cb->size - 1); // must be atomic } The modification
    
    return eErrorNone;
}

enum eError CBWriteOwner(struct sCircularBuffer *cb, unsigned char data, unsigned char owner){
    cb->owner[cb->write] = owner;

    return CBWrite(cb, data);
}

enum eError CBRead(struct sCircularBuffer *cb, unsigned char *data) {
    if (CBLengthData(cb) == 0) { return eErrorBufferEmpty; }
    *data = cb->data[cb->read];
    cb->read++; // = (cb->read + 1) & ( cb->size - 1);

    return eErrorNone;
}

enum eError CBReadOwner(struct sCircularBuffer *cb, unsigned char owner) {
    if(cb->owner[cb->read] == owner) {
        return eErrorNone;
    }

    return eErrorBufferBusy;
}
