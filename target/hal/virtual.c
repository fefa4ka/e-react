#include "virtual.h"
#include <stdio.h>
#include <time.h>

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
static int adc_readConvertion(void *channel);


static void uart_init(void *baudrate);
static bool uart_isDataReceived();
static bool uart_isTransmitReady();
static void uart_transmit(unsigned char data);
static unsigned char uart_receive();


static void timer_init(void *config);
static unsigned int timer_get();
static void timer_set(unsigned int ticks, void(*callback)(void *args), void *args);
static void timer_off();
static unsigned int timer_usFromTicks(unsigned int ticks);

HAL Virtual_HAL = {
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
    },
    .uart = {
        .init = uart_init,
        .isDataReceived = uart_isDataReceived,
        .isTransmitReady = uart_isTransmitReady,
        .transmit = uart_transmit,
        .receive = uart_receive
    },
    .timer = {
        .init = timer_init,
        .get = timer_get,
        .set = timer_set,
        .off = timer_off,
        .usFromTicks = timer_usFromTicks
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

static int 
adc_readConvertion(void *channel) {
    unsigned short *ch = (unsigned short *)channel;
    printf("ADC readConvertion %d\n", *ch); 

    return 41;
}


// UART
static void 
uart_init(void *baudrate) {
    unsigned int baud = *(unsigned int *)baudrate;

    
    printf("UART init baudrate %d\n", baud);
}

static inline bool 
uart_isDataReceived() {
    printf("UART isDataReceived\n");

    return false;
}

static inline bool 
uart_isTransmitReady() {
    return true;
}

static inline void 
uart_transmit(unsigned char data) {
    printf("%c\n", data);
}

static inline unsigned char 
uart_receive() {
    printf("UART receive");
    return 0;
}

/* Time */
static void
timer_init(void *config) {
    srand(time(NULL));
    printf("Timer init\n"); 
}


static inline unsigned int
timer_get() {
    unsigned int second = (unsigned int)clock();
    printf("Timer get: %d\n", second);
    return second; 
}

static void
timer_set(unsigned int ticks, void(*callback)(void *args), void *args) {
    printf("Timer set: %d\n", ticks); 
    callback(args);
}

static void 
timer_off() {
    printf("Timer off\n");
}

static unsigned int timer_usFromTicks(unsigned int ticks) {
    return ticks >> 1;
}

