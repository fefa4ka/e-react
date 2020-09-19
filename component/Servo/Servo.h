
#ifndef Servo_block_h
#define Servo_block_h

#include "../IO/IO.h"
#include "../Scheduler/Scheduler.h"

typedef struct {
    io_handler     *io;
    void           *pin;
    unsigned char  speed;
    unsigned int   angle;
    bool           enabled;
    Component      *scheduler;
} Servo_blockProps;

typedef struct {
    unsigned int           duty_cycle;
    unsigned int           remain_time;
    bool                   on_duty;
    bool                   scheduled;
} Servo_blockState;

React_Header(Servo);

#endif
