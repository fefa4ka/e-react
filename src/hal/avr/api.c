#include "api.h"

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
static unsigned int timer_usFromTicks(unsigned int ticks);


HAL AVR_HAL = {
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
        .getTimer = timer_get,
        .usFromTicks = timer_usFromTicks
    }
};

static inline void
in(void *pin)
{
    AVRPin *Pin = (AVRPin *)pin;
    
    *(Pin->port.ddr) &= ~(1 << (Pin->number));
}

static inline void 
out(void *pin) {
    AVRPin *Pin = (AVRPin *)pin;

    *(Pin->port.ddr) |= (1 << (Pin->number));
}


static inline void 
on(void *pin) {
    AVRPin *Pin = (AVRPin *)pin;

    *(Pin->port.port) |= (1 << (Pin->number));
}

static inline void 
off(void *pin) {
    AVRPin *Pin = (AVRPin *)pin;

    *(Pin->port.port) &= ~(1 << (Pin->number));
}

static inline void 
flip(void *pin) {
    AVRPin *Pin = (AVRPin *)pin;

    *(Pin->port.port) ^= (1 << (Pin->number));
}

static inline void 
pullup(void *pin) {
    on(pin);
}

static inline bool
get(void *pin) {
    AVRPin *Pin = (AVRPin *)pin;

    return *(Pin->port.pin) & (1 << (Pin->number));
}


// ADC 

static void
adc_mount(void *prescaler) {
    // AREF = AVcc
    ADMUX = (1<<REFS0);
 
    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

static inline void
adc_selectChannel(void *channel) {
    char ch = *(char *)channel;
    // char ch = 2;
    // select the corresponding channel 0~7
    // ANDing with ’7′ will always keep the value
    // of ‘ch’ between 0 and 7
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8) | ch; // clears the bottom 3 bits before ORing
}

static inline void
adc_startConvertion(void *channel) {
  // start single convertion
  // write ’1′ to ADSC
  ADCSRA |= (1<<ADSC);
}

static inline bool 
adc_isConvertionReady(void *channel) {
    return (ADCSRA & (1 << ADSC)) == 0;
}

static inline int
adc_readConvertion(void *channel) {
    return (ADC);
}


// UART
static void 
uart_init(void *baudrate) {
    unsigned int baud = *(unsigned int *)baudrate;

    /* Set baud rate */
    UBRR0H = (unsigned char)(baud>>8);
    UBRR0L = (unsigned char)baud;

    UCSR0A |= (1 << U2X0);
    /* Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (3 << UCSZ00);
}

static inline bool 
uart_isDataReceived() {
    return (UCSR0A & (1 << RXC0));
}

static inline bool 
uart_isTransmitReady() {
    return (UCSR0A & (1 << UDRE0));
}

static inline void 
uart_transmit(unsigned char data) {
    UDR0 = data;
}

static inline unsigned char 
uart_receive() {
    return UDR0;
}


/* Time */
static void
timer_init(void *config) {
    TCCR1B |= (1 << CS11);// | (1 << CS10);
}

static inline unsigned int
timer_get() {
    return TCNT1;
}

static inline unsigned int timer_usFromTicks(unsigned int ticks) {
    return ticks >> 1;
}

