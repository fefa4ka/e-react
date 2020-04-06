#ifndef blocks_Button_h
#define blocks_Button_h

#include "../IO/IO.h"
#include "../Time/Time.h"

typedef struct {
    io_handler *io;
    void *pin;
    
    enum {
        push,
        toggle
    } type;
    
    
    rtc_datetime *time;
    int  bounce_delay_ms;

    void (*onPress)(Component *instance);
    void (*onRelease)(Component *instance);
    void (*onToggle)(Component *instance);
} Button_blockProps;

typedef struct {
    bool level : 4;
    bool pressed : 4;
    unsigned long tick;
} Button_blockState;

Component Button_block(Button_blockProps *props, Button_blockState *state);

#endif
