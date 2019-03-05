#include "virtual.h"
#include <stdio.h>

static void in(void *pin);
static void out(void *pin);
static void on(void *pin);
static void off(void *pin);
static void flip(void *pin);
static void pullup(void *pin);
static bool get(void *pin);

static void adc_mount(void *prescaler);
static void adc_selectChannel(void *channel);
static void adc_startConvertion(void *channel);
static bool adc_isConvertionReady(void *channel);
static unsigned short adc_readConvertion(void *channel);


static void uart_init(void *baudrate);
static void uart_puts(char *string);
static void uart_gets(char *buffer, unsigned char bufferlimit);
static void uart_getln(char *buffer, unsigned char bufferlimit);

const HAL Virtual_HAL = {
    .io = {
        .in = in,
        .out = out,
        .on = on,
        .off = off,
        .flip = flip,
        .get = get,
        .pullup = pullup
    },
    .adc = {
        .mount = adc_mount,
        .selectChannel = adc_selectChannel,
        .startConvertion = adc_startConvertion,
        .isConvertionReady = adc_isConvertionReady,
        .readConvertion = adc_readConvertion
    }
};

// IO

static void
in(void *pin) {
    VirtualPin *Pin = (VirtualPin *)pin;

    printf("Pin PORT_%c_%d — in\n", Pin->port, Pin->number);
}

static void 
out(void *pin) {
    VirtualPin *Pin = (VirtualPin *)pin;
    printf("Pin PORT_%c_%d — out\n", Pin->port, Pin->number);
}


static void 
on(void *pin) {
    VirtualPin *Pin = (VirtualPin *)pin;

    printf("Pin PORT_%c_%d — on\n", Pin->port, Pin->number);
}


static void 
off(void *pin) {
    VirtualPin *Pin = (VirtualPin *)pin;

    printf("Pin PORT_%c_%d — off\n", Pin->port, Pin->number);
}


static void 
flip(void *pin) {
    VirtualPin *Pin = (VirtualPin *)pin;

    printf("Pin PORT_%c_%d — flip", Pin->port, Pin->number);
}


static void 
pullup(void *pin) {
    on(pin);
}

static bool
get(void *pin) {
    VirtualPin *Pin = (VirtualPin *)pin;

    printf("Pin PORT_%c_%d — get", Pin->port, Pin->number);

    return true;
}


// ADC

static void
adc_mount(void *prescaler) {
    printf("ADC init\n");
}

static void
adc_selectChannel(void *channel) {
    unsigned short *ch = (unsigned short *)channel;

    printf("ADC selectChannel %d\n", *ch);
}

static void
adc_startConvertion(void *channel) {
    printf("ADC startConvertion\n");
}

static bool 
adc_isConvertionReady(void *channel) {
    unsigned short *ch = (unsigned short *)channel;
    printf("ADC isConvertionReady %d\n", *ch);

    return true;
}

static unsigned short
adc_readConvertion(void *channel) {
    unsigned short *ch = (unsigned short *)channel;
    printf("ADC readConvertion %d\n", *ch); 

    return 41;
}


// UART
