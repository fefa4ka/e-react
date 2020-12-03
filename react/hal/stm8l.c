#include "stm8l.h"

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
static void timer_off();
static unsigned int timer_usFromTicks(unsigned int ticks);


HAL hw = {
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
        .off = timer_off,
        .usFromTicks = timer_usFromTicks
    }
};

static inline void
in(void *pin)
{
    pin_t *Pin = (pin_t *)pin;
    
    *(Pin->port.ddr) &= ~(1 << (Pin->number));
}

static inline void 
out(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    *(Pin->port.ddr) |= (1 << (Pin->number));
    *(Pin->port.pin) |= (1 << (Pin->number));
}


static inline void 
on(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    *(Pin->port.port) |= (1 << (Pin->number));
}

static inline void 
off(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    *(Pin->port.port) &= ~(1 << (Pin->number));
}

static inline void 
flip(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    *(Pin->port.port) ^= (1 << (Pin->number));
}

static inline void 
pullup(void *pin) {
    on(pin);
}

static inline bool
get(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    return *(Pin->port.pin) & (1 << (Pin->number));
}


// ADC 

static void
adc_mount(void *prescaler) {
}

static inline void
adc_selectChannel(void *channel) {
}

static inline void
adc_startConvertion(void *channel) {

}

static inline bool 
adc_isConvertionReady(void *channel) {
    return true;
}

static inline int
adc_readConvertion(void *channel) {
    return 0;
}


// UART
static void 
uart_init(void *baudrate) {
}

static inline bool 
uart_isDataReceived() {
    return false;
}

static inline bool 
uart_isTransmitReady() {
    return false;
}

static inline void 
uart_transmit(unsigned char data) {
}

static inline unsigned char 
uart_receive() {
    return 0;
}


// SPI 
static void 
spi_init(void *config) {
}

static inline bool 
spi_isDataReceived() {
    return true;
}

static inline bool 
spi_isTransmitReady() {
    return false;
}

static inline void 
spi_transmit(unsigned char data) {
}

static inline unsigned char 
spi_receive() {
    return 0;
}


/* Time */
static void
timer_init(void *config) {
}

static inline unsigned int
timer_get() {
    return 0;
}

void (*_timer_func)(void *args);
void *_timer_func_args = 0;



static void 
timer_off() {
}

static unsigned int timer_usFromTicks(unsigned int ticks) {
    return 0;
}

