#ifndef config_device_h
#define config_device_h

#define BAUD  9600

#define UBRR_VALUE (((F_CPU) + 4UL * (BAUD)) / (8UL * (BAUD)) -1UL)

#define DEBUG true 

#endif
