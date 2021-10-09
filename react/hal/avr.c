#include "avr.h"
#include "hash.h"

void io_in(void *pin);
void io_out(void *pin);
void io_on(void *pin);
void io_off(void *pin);
void io_flip(void *pin);
void io_pullup(void *pin);
bool io_get(void *pin);

void adc_mount(void *prescaler);
void adc_selectChannel(void *channel);
void adc_startConvertion(void *channel);
bool adc_isConvertionReady(void *channel);
int adc_readConvertion(void *channel);

void uart_init(void *baudrate);
bool uart_isDataReceived();
bool uart_isTransmitReady();
void uart_transmit(unsigned char data);
unsigned char uart_receive();

void timer_init(void *config);
unsigned int timer_get();
void timer_set(unsigned int ticks, void(*callback)(void *args), void *args);
void timer_off();
unsigned int timer_usFromTicks(unsigned int ticks);


HAL hw = {
    .io = {
        .in = io_in,
        .out = io_out,
        .on = io_on,
        .off = io_off,
        .flip = io_flip,
        .get = io_get,
        .pullup = io_pullup
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

void
io_in(void *pin)
{
    pin_t *Pin = (pin_t *)pin;

    bit_clear(*Pin->port.ddr, Pin->number);
    bit_set(*Pin->port.port, Pin->number);
}

void
io_out(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    bit_set(*Pin->port.ddr, Pin->number);
}


void
io_on(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    bit_set(*Pin->port.port, Pin->number);
}

void
io_off(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    bit_clear(*Pin->port.port, Pin->number);
}

void
io_flip(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    bit_flip(*Pin->port.port, Pin->number);
}

void
io_pullup(void *pin) {
    io_on(pin);
}

bool
io_get(void *pin) {
    pin_t *Pin = (pin_t *)pin;

    return *(Pin->port.pin) & (1 << (Pin->number));
}

/*
#define HW_ISR_VECTOR_SIZE 8
unsigned int isr_index[HW_ISR_VECTOR_SIZE];
struct callback isr_handlers[HW_ISR_VECTOR_SIZE];

union isr_vector_index {
    struct {
        char port;
        char num;
    } pin;
    unsigned int index;
};

struct hash_table isr_vectors = {
    .index = isr_index,
    .data = (void **)&isr_handlers,
    .size = HW_ISR_VECTOR_SIZE
};

void
io_isr_handler(char vector) {
     for(unsigned int index = 0; index <= isr_vectors.used; index++) {
        union isr_vector_index vector_index;
        vector_index.index = isr_vectors.index[index];
        if((vector_index.pin.port ^ vector) == 0) {
            struct callback *isr_handler = isr_vectors.data[index];
            isr_handler->method(0, isr_handler->argument);
        }
    }
}
*/




/*
 *  The pin change interrupt PCI1 will trigger if any enabled PCINT14..8 pin toggles.
 *  The pin change interrupt PCI0 will trigger if any enabled PCINT7..0 pin toggles.
*/
void
io_isr_mount(void *pin, struct callback *callback) {
//    pin_t *Pin = (pin_t *)pin;
//    union isr_vector_index vector_index = { .pin = { .num = 2 } };
//    /* INT0 */
//    if(Pin->port.pin == &PIND && Pin->number == 2) {
//        vector_index.pin.port = 'A';
//    } else if() {
//    }

//    hash_write(&isr_vectors, vector_index.index, callback);
    /* INT1 */
    /* PCINT7..0 */
    /* Enabled interrupt, enable pin */
    /* When mount for different ports one interrupt we need list of vectros */
    /* Dispatcher needed for each ISR 8 pointers */
    /* Save index as high byte use port and low pin */
}




// ADC

void
adc_mount(void *prescaler) {
    // AREF = AVcc
    ADMUX = (1<<REFS0);

    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

inline void
adc_selectChannel(void *channel) {
    char ch = *(char *)channel;
    // char ch = 2;
    // select the corresponding channel 0~7
    // ANDing with ’7′ will always keep the value
    // of ‘ch’ between 0 and 7
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8) | ch; // clears the bottom 3 bits before ORing
}

inline void
adc_startConvertion(void *channel) {
  // start single convertion
  // write ’1′ to ADSC
  ADCSRA |= (1<<ADSC);
}

inline bool
adc_isConvertionReady(void *channel) {
    return (ADCSRA & (1 << ADSC)) == 0;
}

inline int
adc_readConvertion(void *channel) {
    return (ADC);
}


// UART
void
uart_init(void *baudrate) {
    unsigned int baud = hw_uart_baudrate(*(unsigned int*)baudrate);

    /* Set baud rate */
    UBRR0H = (unsigned char)(baud>>8);
    UBRR0L = (unsigned char)baud;

    UCSR0A |= (1 << U2X0);
    /* Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (3 << UCSZ00);
}

inline bool
uart_isDataReceived() {
    return (UCSR0A & (1 << RXC0));
}

inline bool
uart_isTransmitReady() {
    return (UCSR0A & (1 << UDRE0));
}

inline void
uart_transmit(unsigned char data) {
    UDR0 = data;
}

inline unsigned char
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
void
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

inline bool
spi_isDataReceived() {
    if(!(SPI_SPSR & (1 << SPI_SPIF))) {
        SPI_SPDR = 0xFF;
    } else {
        return true;
    }
}

inline bool
spi_isTransmitReady() {
    return !(SPI_SPSR & (1 << SPI_SPIF));
}

inline void
spi_transmit(unsigned char data) {
    SPI_SPDR = data;
}

inline unsigned char
spi_receive() {
    return SPI_SPDR;
}


/* Time */
void
timer_init(void *config) {
    TCCR1B |= (1 << CS11);// | (1 << WGM12);// | (1 << CS10);
}

inline unsigned int
timer_get() {
    return TCNT1;
}

void (*_timer_func)(void *args);
void *_timer_func_args = 0;

ISR(TIMER1_COMPA_vect) {
	_timer_func(_timer_func_args);
}

void
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

void
timer_off() {
    TIMSK1 &= ~(1 << OCIE1A); // Disable interrupts
}

unsigned int timer_usFromTicks(unsigned int ticks) {
    return ticks >> 1;
}

