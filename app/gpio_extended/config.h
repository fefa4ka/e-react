#pragma once

#include <Scheduler.h>
#include <Button.h>
#include <Scheduler.h>
#include <Bitbang.h>

#define   VERSION      1

#define   BAUDRATE     9600

#define   BUFFER_SIZE          8


struct device
{
    struct rtc_datetime    time;
    struct events_queue    scheduler; 

    struct ring_buffer     input_buffer;
    struct ring_buffer     output_buffer;
    
    unsigned char          index; 

    pin_t                  clk_pin;
    
    pin_t                  miso_pin;
    pin_t                  mosi_pin;
};
