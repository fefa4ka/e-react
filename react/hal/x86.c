#include "x86.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

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
static int  adc_readConvertion(void *channel);


static void          uart_init(void *baudrate);
static bool          uart_isDataReceived();
static bool          uart_isTransmitReady();
static void          uart_transmit(unsigned char data);
static unsigned char uart_receive();


static void         timer_init(void *config);
static unsigned int timer_get();
static void         timer_set(unsigned int ticks, void (*callback)(void *args),
                              void *       args);
static void         timer_off();
static unsigned int timer_usFromTicks(unsigned int ticks);

HAL hw = {.io    = {.in     = in,
                 .out    = out,
                 .on     = on,
                 .off    = off,
                 .flip   = flip,
                 .get    = get,
                 .pullup = pullup,},
          .adc   = {.mount             = adc_mount,
                  .selectChannel     = adc_selectChannel,
                  .startConvertion   = adc_startConvertion,
                  .isConvertionReady = adc_isConvertionReady,
                  .readConvertion    = adc_readConvertion,},
          .uart  = {.init            = uart_init,
                   .isDataReceived  = uart_isDataReceived,
                   .isTransmitReady = uart_isTransmitReady,
                   .transmit        = uart_transmit,
                   .receive         = uart_receive,},
          .timer = {.init        = timer_init,
                    .get         = timer_get,
                    .set         = timer_set,
                    .off         = timer_off,
                    .usFromTicks = timer_usFromTicks,},};

// IO
/**
 * Larger the number, larger will the table
 * If the table size is too large, elements distirubtion will be too widespread
 * and makes the table to be not utilized efficiently
 *
 * Hence choose the table size judiciously
 *
 * Better choice would be the next prime number after the maximum number of
 * unique elements eg. If total number of unique elements are 10,000: TABLE_SIZE
 * could be 10007
 */
#define MAX_TABLE_SIZE 10007 // Prime Number
unsigned int pins_index[128];
pin_t *      pins_buffer[128];

struct hash_table pins = {
    .index = pins_index,
    .data  = (void **)&pins_buffer,
    .size  = 128,
    .used  = 0,
};

unsigned int hash_pin(pin_t *pin)
{
    unsigned long hash = 0;
    char *        word = pin->name;

    while (*word != '\0') {
        hash += *word++;
    }

    hash += pin->number;

    return (hash % MAX_TABLE_SIZE);
}

static pin_t * get_pin(pin_t *pin)
{
    pin_t *Pin;
    if (hash_read(&pins, hash_pin(pin), (void **)&Pin) == ERROR_NONE) {
        //printf("Pin PORT_%s_%d — cached\r\n", Pin->name, Pin->number);
        return Pin;
    }

    Pin = malloc(sizeof(pin_t));
    *Pin = *pin;
    Pin->name = strdup(pin->name);

    //printf("%s - %d - %d\n", pin->name, hash_pin(pin), hash_pin(Pin));
    hash_write(&pins, hash_pin(Pin), Pin);

    //printf("Pin PORT_%s_%d — new\r\n", Pin->name, Pin->number);
    return Pin;
}

void free_pins()
{
    for (unsigned int index = 0; index < pins.used; index++) {
        pin_t *Pin= pins.data[index];
        free(Pin->name);
        free(Pin);
    }
}

static void in(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    bit_clear(Pin->port.ddr, Pin->number);
    bit_set(Pin->port.port, Pin->number);

    // printf("Pin PORT_%s_%d — in\r\n", Pin->port, Pin->number);
}

static void out(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    bit_set(Pin->port.ddr, Pin->number);

    //printf("Pin PORT_%s_%d — out\r\n", Pin->port, Pin->number);
}


static void on(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    bit_set(Pin->port.port, Pin->number);
    //printf("Pin PORT_%s_%d — on - %d - %d\r\n", Pin->name, Pin->number, Pin->port.port, 1 << Pin->number);
}


static void off(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    bit_clear(Pin->port.port, Pin->number);
    //printf("Pin PORT_%s_%d — off - %d\r\n", Pin->name, Pin->number, Pin->port.port);
}


static void flip(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    bit_flip(Pin->port.port, Pin->number);
    // printf("Pin PORT_%s_%d — flip\r\n", Pin->port, Pin->number);
}


static void pullup(void *pin) { on(pin); }

static bool get(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    //printf("Pin PORT_%s_%d — get - %d - %d\r\n", Pin->name, Pin->number, Pin->port.port, 1 << Pin->number);
    return (Pin->port.port) & (1 << Pin->number);
}


// ADC

static void adc_mount(void *prescaler) {
    srand(time(NULL));
    //printf("ADC init\r\n");
}

static void adc_selectChannel(void *channel)
{
    //unsigned short *ch = (unsigned short *)channel;

    //printf("ADC selectChannel %d\r\n", *ch);
}

static void adc_startConvertion(void *channel)
{
    //printf("ADC startConvertion\r\n");
}

static bool adc_isConvertionReady(void *channel)
{
    //unsigned short *ch = (unsigned short *)channel;
    //printf("ADC isConvertionReady %d\r\n", *ch);

    return true;
}

static int adc_readConvertion(void *channel)
{
    //unsigned short *ch = (unsigned short *)channel;
    //printf("ADC readConvertion %d\r\n", *ch);

    return rand();
}


// UART
static void uart_init(void *baudrate)
{
    unsigned int baud = *(unsigned int *)baudrate;


    printf("UART init baudrate %d\r\n", baud);
}

static inline bool uart_isDataReceived()
{
    //printf("UART isDataReceived\r\n");

    return false;
}

static inline bool uart_isTransmitReady() { return true; }

static inline void uart_transmit(unsigned char data) { putchar(data); }

static inline unsigned char uart_receive()
{
    char c = getchar();
    return c;
}

/* Time */
static void timer_init(void *config)
{
    srand(time(NULL));
    printf("Timer init\r\n");
}


static inline unsigned int timer_get()
{
    clock_t tick = clock();
    //printf("Timer get: %ld, %ld, %d\r\n", tick, CLOCKS_PER_SEC, tick / CLOCKS_PER_SEC);
    return (unsigned int)tick;
}

struct timer_callback {
    unsigned int timeout;
    void (*callback)(void *args);
    void *args;
    size_t index;
    pthread_t thread;
};

#define TIMERS_NR 10
struct timer_callback timer_callback_buffer[TIMERS_NR] = {0};

void *timer_timeout(void *ptr) {
    struct timer_callback *callback = ptr;

    usleep(callback->timeout);
    callback->callback(callback->args);
    *callback = (struct timer_callback){0};

    return NULL;
}

static void timer_set(unsigned int ticks, void (*callback)(void *args),
                      void *       args)
{
    for(size_t i = 0; i < TIMERS_NR; i++) {
        if(timer_callback_buffer[i].callback == NULL) {
            struct timer_callback timer_callback = { ticks, callback, args, i };
            struct timer_callback *timer = timer_callback_buffer + i;
            *timer = timer_callback;
            pthread_create(&timer->thread, NULL, *timer_timeout, (void *)timer);

            printf("Timer #%ld set: %d\r\n", i, ticks);
            break;
        }
    }

}

static void timer_off() { printf("Timer off\r\n"); }

#define CLOCKS_PER_USEC (CLOCKS_PER_SEC / 1000000)
static unsigned int timer_usFromTicks(unsigned int ticks) { return ticks / CLOCKS_PER_USEC; }
