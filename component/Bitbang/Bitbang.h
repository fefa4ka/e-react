
#pragma once 

#include <stdbool.h>
#include <component.h>
#include <circular.h>
#include <hal.h>
#include <Calendar.h>

#define BITBANG_MAX_LINES 8

typedef struct
{
    io_handler *io;
    rtc_datetime_t   *time;
    
    unsigned int  baudrate;
    void          *pins;
    void          *clock;
    pin_mode_t    *modes;
    struct ring_buffer_s 
                  *buffers;

    void (*onStart)(Component *instance);
    void (*onReceive)(Component *instance);
    void (*onReceiveLine)(Component *instance);
    void (*onTransmit)(Component *instance);
    void (*onTransmitLine)(Component *instance);
} Bitbang_blockProps;

typedef struct {
    bool            sending;
    short           position;
    unsigned char   data[BITBANG_MAX_LINES]; 
    unsigned long   tick;
} Bitbang_blockState;


React_Header(Bitbang);
