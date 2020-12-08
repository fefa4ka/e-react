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
    *(Pin->port.pin) |= (1 << (Pin->number));
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

    return (*(Pin->port.state) >> (Pin->number)) & 0x1;
}


// ADC 

static void
adc_mount(void *prescaler) {

    sfr_CLK.PCKENR2.PCKEN20 = 1;
  
  // set ADC clock to 1/18*fMaster to minimize influence between channels 
  // Conversion takes 14 cycl -> 16µs
  sfr_ADC1.CR1.ADON = 1;
  
  
  // disable all Schmitt trigger, which is recommended for EMC noisy conditions (skip for simplicity)
  //sfr_ADC1.TDRH.byte = 0xFF;
  //sfr_ADC1.TDRL.byte = 0xFF;

    sfr_ADC1.CR1.ADON = 1;
    sfr_ADC1.SQR1.DMAOFF = 1;
    /*
	// 384 (max) adc clock cycles sampling time
	// Channels 1-24
	ADC1_CR2 = 0x02;
	// Channel 24 Vrefint and TS
	ADC1_CR3 = 0xE0;
    */
}

static inline void
adc_selectChannel(void *channel) {
#define adc_channel *(int *)channel
	sfr_ADC1.SQR2.byte = 0;
	sfr_ADC1.SQR3.byte = 0;
	sfr_ADC1.SQR4.byte = 0;
	
	if (adc_channel > 15)
	{
		sfr_ADC1.SQR2.byte = (1 << (adc_channel - 16));
	}
	else if (adc_channel > 7)
	{
		sfr_ADC1.SQR3.byte = (1 << (adc_channel - 8));
	}
	else
	{
		sfr_ADC1.SQR4.byte = (1 << adc_channel);
	}

	ADC1_CR1 |= 0x02;
#undef adc_channel
}

static inline void
adc_startConvertion(void *channel) {
	ADC1_CR1 |= ADC1_CR1_START;
}

static inline bool 
adc_isConvertionReady(void *channel) {
    return (ADC1_SR & 0x01);
}

static inline int
adc_readConvertion(void *channel) {
    return (ADC1_DRH << 8) | ADC1_DRL;
}


// UART
static void 
uart_init(void *baudrate) {
    /* round to nearest integer */
    sfr_CLK.CKDIVR.byte = 0x00;
    sfr_CLK.PCKENR1.PCKEN15 = 1;
      
    PC_DDR &= ~(1 << 2);  //PC2 RX USART1 receive (вход)
    PC_DDR|=1<<3; //PC3 TX USART1 transmit (выход)
      // set UART behaviour
      sfr_USART1.CR1.byte = 0x00;       // enable UART, 8 data bits, no parity control
      sfr_USART1.CR2.byte = 0x00;       // no interrupts, disable sender/receiver 
      sfr_USART1.CR3.byte = 0x00;       // no LIN support, 1 stop bit, no clock output(?)

      // set baudrate (note: BRR2 must be written before BRR1!)
      unsigned int baud;
      baud = (unsigned long) (((unsigned long) 16000000L)/(*(unsigned int*)baudrate));
      sfr_USART1.BRR2.byte = (uint8_t) (((baud & 0xF000) >> 8) | (baud & 0x000F));
      sfr_USART1.BRR1.byte = (uint8_t) ((baud & 0x0FF0) >> 4);
      
      // enable transmission, no transmission
      sfr_USART1.CR2.REN  = 1;  // enable receiver
      sfr_USART1.CR2.TEN  = 1;  // enable sender
      //sfr_UART.CR2.TIEN = 1;  // enable transmit interrupt
      //sfr_UART.CR2.RIEN = 1;  // enable receive interrupt
}

/*
static inline bool 
uart_isDataReceived() {
    return sfr_USART1.SR.TC;
}

static inline bool 
uart_isTransmitReady() {
    return sfr_USART1.SR.TXE;
}

static inline void 
uart_transmit(unsigned char data) {
    sfr_USART1.DR.byte = data;
}

static inline unsigned char 
uart_receive() {
    return sfr_USART1.DR.byte;
}
*/


static inline bool 
uart_isDataReceived() {
    return (USART1_SR & (1 << USART1_SR_RXNE));
}

static inline bool 
uart_isTransmitReady() {
    return (USART1_SR & (1 << USART1_SR_TC));
}

static inline void 
uart_transmit(unsigned char data) {
    USART1_DR = data;
}

static inline unsigned char 
uart_receive() {
    return USART1_DR;
}



// SPI 
static void 
spi_init(void *config) {
  // SPI port setup: MISO is pullup in, MOSI & SCK are push-pull out
  sfr_PORTC.DDR.byte |= (uint8_t) (PIN5 | PIN6);     // input(=0) or output(=1)
  sfr_PORTC.CR1.byte |= (uint8_t) (PIN5 | PIN6);     // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull

  // CS/SS (PD2) as output
  sfr_PORTD.DDR.DDR2 = 1;     // input(=0) or output(=1)
  sfr_PORTD.CR1.C12  = 1;     // input: 0=float, 1=pull-up; output: 0=open-drain, 1=push-pull
//  CSN_RELEASE;                // CS high

  // SPI registers: First reset everything
  sfr_SPI1.CR1.byte = 0x00;
  sfr_SPI1.CR2.byte = 0x00;

  // SPI_CR1 LSBFIRST=0 (MSB is transmitted first)
  sfr_SPI1.CR1.LSBFIRST = 0;
    
  // Baud Rate Control: 0b111 = fmaster / 256 (62,500 baud)
  sfr_SPI1.CR1.BR = 7;
  
  // Clock Phase, The first clock transition is the first data capture edge
  sfr_SPI1.CR1.CPOL = 0;
  
  // Clock Polarity, SCK=0 when idle
  sfr_SPI1.CR1.CPHA = 0;
  
  sfr_SPI1.CR2.SSM  = 1;       // Software slave management, enabled
  sfr_SPI1.CR2.SSI  = 1;       // Internal slave select, Master mode
  sfr_SPI1.CR1.MSTR = 1;       // Master configuration

  sfr_SPI1.CR1.SPE  = 1;       // SPI Enable, Peripheral enabled
}

static inline bool 
spi_isDataReceived() {
    return sfr_SPI1.SR.RXNE;
}

static inline bool 
spi_isTransmitReady() {
    return !sfr_SPI1.SR.BSY && sfr_SPI1.SR.TXE;
}

static inline void 
spi_transmit(unsigned char data) {
  sfr_SPI1.CR1.MSTR = 1;       // Master device.
  sfr_SPI1.DR.byte  = data;    // send 1B
}

static inline unsigned char 
spi_receive() {
    return sfr_SPI1.DR.byte;     // read data
}


/* Time */
static void
timer_init (void *config)
{
    sfr_CLK.CKDIVR.byte = 0x00;
	CLK_PCKENR2 |= 0x02; // Enable clock to timer

	// Configure timer
	// 1000 ticks per second
	TIM1_PSCRH = 0x00;
	TIM1_PSCRL = 0x07;

	// Enable timer
    sfr_TIM1.CR1.CEN     = 1;                               // start TIM1
}

static inline unsigned int
timer_get ()
{
	unsigned char h = TIM1_CNTRH;
	unsigned char l = TIM1_CNTRL;
	return((unsigned int)(h) << 8 | l);

}

void (*_timer_func)(void *args);
void *_timer_func_args = 0;

void timer1_isr(void) __interrupt(_TIM1_CAPCOM_CC1IF_VECTOR_) {
	_timer_func(_timer_func_args);
}

static void
timer_set (unsigned int ticks, void (*callback) (void *args), void *args)
{
    // disable interrupts
    DISABLE_INTERRUPTS ();
    _timer_func        = callback;
    _timer_func_args   = args;

    sfr_TIM1.CCMR1.byte = ticks;
    

    //INT_TIM1FLAG = INT_TIMCC1IF;
    //INT_TIM1CFG |= INT_TIMCC1IF;        /* Compare 1 interrupt enable. */

    // enable interrupts
    ENABLE_INTERRUPTS ();
}

static void
timer_off ()
{
    //INT_TIM1CFG &= ~INT_TIMCC1IF;       /* Disable the next compare interrupt */
}

static unsigned int
timer_usFromTicks (unsigned int ticks)
{
    return ticks >> 1;
}
