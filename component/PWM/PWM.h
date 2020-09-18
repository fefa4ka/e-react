#ifndef PWM_block_h
#define PWM_block_h

#include <IO.h>
#include <Time.h>

typedef struct {
    io_handler     *io;
    void           *pin;
    unsigned int   frequency;
    unsigned char  duty_cycle;
    rtc_datetime_t *time;
} PWM_blockProps;

typedef struct {
    volatile unsigned long tick;
    bool          on_duty;
} PWM_blockState;

React_Header(PWM);

#endif
