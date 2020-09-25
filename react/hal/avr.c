#include "avr.h"

#define log_pin(port, pin) \
    DDR##port |= (1 << pin); \
    PORT##port ^= (1 << pin); \
    PORT##port ^= (1 << pin);

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

static void uart_init(unsigned int baudrate);
static bool uart_isDataReceived();
static bool uart_isTransmitReady();
static void uart_transmit(unsigned char data);
static unsigned char uart_receive();

static void timer_init(void *config);
static unsigned int timer_get();
static void timer_set(unsigned int ticks, void(*callback)(void *args), void *args);
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
        .set = timer_set,
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
uart_init(unsigned int baudrate) {
    unsigned int baud = hw_uart_baudrate(baudrate);

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


// SPI 
#define SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define SPI_MISO	PORTB4
#define SPI_MOSI	PORTB3
#define SPI_SCK		PORTB5
#define SPI_SS		PORTB2

#define SPI_SPCR	SPCR
#define SPI_SPDR	SPDR
#define SPI_SPSR	SPSR
#define SPI_SPIF	SPIF
#define SPI_SPE		SPE
#define SPI_MSTR	MSTR
#define SPI_SPR0	SPR0
#define SPI_SPR1	SPR1
static void 
spi_init(void *config) {
	// make the MOSI, SCK, and SS pins outputs
	SPI_DDR |= ( 1 << SPI_MOSI ) | ( 1 << SPI_SCK ) | ( 1 << SPI_SS );

	// make sure the MISO pin is input
	SPI_DDR &= ~( 1 << SPI_MISO );

	// set up the SPI module: SPI enabled, MSB first, master mode,
	//  clock polarity and phase = 0, F_osc/16
	SPI_SPCR = ( 1 << SPI_SPE ) | ( 1 << SPI_MSTR );// | ( 1 << SPI_SPR0 );
	SPI_SPSR = 1;     // set double SPI speed for F_osc/2
}

static inline bool 
spi_isDataReceived() {
    if(!(SPI_SPSR & (1 << SPI_SPIF))) {
        SPI_SPDR = 0xFF;
    } else {
        return true;
    }
}

static inline bool 
spi_isTransmitReady() {
    return !(SPI_SPSR & (1 << SPI_SPIF));
}

static inline void 
spi_transmit(unsigned char data) {
    SPI_SPDR = data;
}

static inline unsigned char 
spi_receive() {
    return SPI_SPDR;
}


/* Time */
static void
timer_init(void *config) {
    TCCR1B |= (1 << CS11);// | (1 << WGM12);// | (1 << CS10);
}

static inline unsigned int
timer_get() {
    return TCNT1;
}

void (*_timer_func)(void *args);
void *_timer_func_args = 0;

ISR(TIMER1_COMPA_vect) {
	_timer_func(_timer_func_args);
}

static void
timer_set(unsigned int ticks, void(*callback)(void *args), void *args) {
    cli();
    TIMSK1 &= ~(1 << OCIE1A); // Disable interrupts

    _timer_func = callback;
    _timer_func_args = args;
    
    OCR1A = ticks;

    TIMSK1 |= 1 << OCIE1A;
    TIFR1 = (1 << OCF1A);
    sei();
}

static void 
timer_off() {
    TIMSK1 &= ~(1 << OCIE1A); // Disable interrupts
}

static unsigned int timer_usFromTicks(unsigned int ticks) {
    return ticks >> 1;
}

