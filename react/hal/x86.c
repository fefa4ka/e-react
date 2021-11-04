#include "x86.h"
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static void gpio_in(void *pin);
static void gpio_out(void *pin);
static void gpio_on(void *pin);
static void gpio_off(void *pin);
static void gpio_flip(void *pin);
static void gpio_pullup(void *pin);
static bool gpio_get(void *pin);

static void    adc_mount(void *prescaler);
static void    adc_selectChannel(void *channel);
static void    adc_startConvertion(void *channel);
static bool    adc_isConvertionReady(void *channel);
static int16_t adc_readConvertion(void *channel);


static void          uart_init(void *baudrate);
static bool          uart_isDataReceived();
static bool          uart_isTransmitReady();
static void          uart_transmit(unsigned char data);
static unsigned char uart_receive();


static void     timer_init(void *config);
static uint16_t timer_get();
static void timer_set(uint16_t ticks, void (*callback)(void *args), void *args);
static void timer_off();
static uint16_t timer_usFromTicks(uint16_t ticks);

HAL hw = {.io    = {.in     = gpio_in,
                    .out    = gpio_out,
                    .on     = gpio_on,
                    .off    = gpio_off,
                    .flip   = gpio_flip,
                    .get    = gpio_get,
                    .pullup = gpio_pullup,},
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
pthread_mutex_t get_pin_lock;

static char  pin_index = 33;
unsigned int pins_index[128];
pin_t *      pins_buffer[128];

struct hash_table pins = {
    .index = pins_index,
    .data  = (void **)&pins_buffer,
    .size  = 128,
    .used  = 0,
};

FILE *log_file = NULL;

FILE *vcd_file;
FILE *vcd_file_log;

void log_init()
{
    if(!log_file)
        log_file = fopen("profiler.log", "w");

    if (log_file == NULL) {
        printf("Error: can't open profiler.log file");
        exit(1);
    }
}

void log_printf(char *message, ...)
{
    va_list args;
    va_start(args, message);
    vfprintf(log_file, message, args);
    va_end(args);
}

void log_clean() { fclose(log_file); }

#define vcd_open(filename) fopen(#filename ".vcd", "w")
void vcd_init()
{
    // use appropriate location if you are using MacOS or Linux
    vcd_file     = vcd_open(profiler);
    vcd_file_log = vcd_open(profiler_log);

    if (vcd_file == NULL) {
        printf("Error: can't open vcd file");
        exit(1);
    }

    if (vcd_file_log == NULL) {
        printf("Error: can't open vcd file");
        exit(1);
    }
}

void vcd_clean()
{
    char c;
    fprintf(vcd_file, "$date"
                      "\n"
                      "   October 17, 2021."
                      "\n"
                      "$end"
                      "\n"
                      "$version"
                      "\n"
                      "   VCD generator tool version info text."
                      "\n"
                      "$end"
                      "\n"
                      "$comment"
                      "\n"
                      "   Any comment text."
                      "\n"
                      "$end"
                      "\n"
                      "$timescale 1us $end"
                      "\n"
                      "$scope module logic $end\n");
    for (unsigned int index = 0; index < pins.used; index++) {
        pin_t *Pin = pins.data[index];
        fprintf(vcd_file, "$var wire 1 %c %s_%d$end\n", Pin->index, Pin->name,
                Pin->number);
    }

    fprintf(vcd_file, "$upscope $end"
                      "\n"
                      "$enddefinitions $end"
                      "\n");
    fclose(vcd_file_log);
    vcd_file_log = fopen("profiler_log.vcd", "r");
    while ((c = fgetc(vcd_file_log)) != EOF)
        fputc(c, vcd_file);
    fclose(vcd_file_log);
    remove("profiler_log.vcd");
    fclose(vcd_file);
}

unsigned int hash_pin(pin_t *pin)
{
    unsigned long hash = 0;
    char *        word = pin->name;

    while (*word != '\0') {
        hash = *word++ + 31 * hash;
    }
    hash += pin->number;

    // log_info("%s_%d=%d", pin->name, pin->number, hash % MAX_TABLE_SIZE);
    return (hash % MAX_TABLE_SIZE);
}

void gpio_thread() {}
void gpio_init()
{
    log_init();
    vcd_init();
    pthread_mutex_init(&get_pin_lock, NULL);
    // TODO: thread with gpio manipulation
}

static pin_t *get_pin(pin_t *pin)
{
    pin_t *Pin;
    pthread_mutex_lock(&get_pin_lock);
    if (hash_read(&pins, hash_pin(pin), (void **)&Pin) == ERROR_NONE) {
        pthread_mutex_unlock(&get_pin_lock);
        // printf("Pin PORT_%s_%d — cached\r\n", Pin->name, Pin->number);
        return Pin;
    }

    Pin            = malloc(sizeof(pin_t));
    Pin->name      = strdup(pin->name);
    Pin->number    = pin->number;
    Pin->port.port = 0;
    Pin->port.pin  = 0;
    Pin->port.ddr  = 0;

    // printf("%s - %d - %d\n", pin->name, hash_pin(pin), hash_pin(Pin));
    hash_write(&pins, hash_pin(Pin), Pin);

    Pin->index = pin_index++;
    // printf("Pin PORT_%s_%d = (#%d %x) new\r\n", Pin->name, Pin->number,
    // hash_pin(pin), &(Pin->port));
    pthread_mutex_unlock(&get_pin_lock);
    return Pin;
}

void dump_pin(pin_t *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);
    printf("Pin PORT_%s_%d #%d = %d\n", Pin->name, Pin->number, hash_pin(pin),
           gpio_get(pin));
}

void free_pins()
{
    printf("exit\n");
    for (unsigned int index = 0; index < pins.used; index++) {
        pin_t *Pin = pins.data[index];
        free(Pin->name);
        free(Pin);
    }
    pthread_mutex_destroy(&get_pin_lock);
    vcd_clean();
    log_clean();
    system("/bin/stty cooked");
}

static void gpio_in(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    bit_clear(Pin->port.ddr, Pin->number);
    bit_set(Pin->port.port, Pin->number);

    REACT_PROFILER_COUNT_LOG(gpio_in, "(%s_%d)", Pin->name, Pin->number);
    // printf("Pin PORT_%s_%d — in\r\n", Pin->port, Pin->number);
}

static void gpio_out(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    bit_set(Pin->port.ddr, Pin->number);

    REACT_PROFILER_COUNT_LOG(gpio_out, "(%s_%d)", Pin->name, Pin->number);
    // printf("Pin PORT_%s_%d — out\r\n", Pin->port, Pin->number);
}


static void gpio_on(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    clock_t current_time = clock();
    if (!((Pin->port.pin) & (1 << Pin->number))) {
        fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, 1, Pin->index);
    }
    bit_set(Pin->port.pin, Pin->number);

    // printf("On %x ", &(Pin->port));
    // dump_pin(pin);
    // printf("Pin PORT_%s_%d — on - %d - %d\r\n", Pin->name, Pin->number,
    // Pin->port.port, 1 << Pin->number);
    REACT_PROFILER_COUNT_LOG(gpio_on, "(%s_%d)", Pin->name, Pin->number);
}


static void gpio_off(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    clock_t current_time = clock();
    if (((Pin->port.pin) & (1 << Pin->number))) {
        fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, 0, Pin->index);
    }
    bit_clear(Pin->port.pin, Pin->number);

    // printf("Off %x ", &(Pin->port));
    // dump_pin(pin);
    // printf("Pin PORT_%s_%d — off - %d\r\n", Pin->name, Pin->number,
    // Pin->port.port);
    REACT_PROFILER_COUNT_LOG(gpio_off, "(%s_%d)", Pin->name, Pin->number);
}


static void gpio_flip(void *pin)
{
    pin_t * Pin          = get_pin((pin_t *)pin);
    clock_t current_time = clock();

    bit_flip(Pin->port.pin, Pin->number);
    fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, gpio_get(pin),
            Pin->index);
    REACT_PROFILER_COUNT_LOG(gpio_flip, "(%s_%d)", Pin->name, Pin->number);
}


static void gpio_pullup(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    bit_set(Pin->port.pin, Pin->number);
    REACT_PROFILER_COUNT_LOG(gpio_pullup, "(%s_%d)", Pin->name, Pin->number);
}

static bool gpio_get(void *pin)
{
    pin_t *Pin = get_pin((pin_t *)pin);

    REACT_PROFILER_COUNT_LOG(gpio_get, "(%s_%d)=%d", Pin->name, Pin->number,
            (Pin->port.pin) & (1 << Pin->number));

    return (Pin->port.pin) & (1 << Pin->number);
}


// ADC

static void adc_mount(void *prescaler)
{
    srand(time(NULL));
    // printf("ADC init\r\n");
    REACT_PROFILER_COUNT(adc_mount);
}

static void adc_selectChannel(void *channel)
{
    // unsigned short *ch = (unsigned short *)channel;

    // printf("ADC selectChannel %d\r\n", *ch);
    REACT_PROFILER_COUNT(adc_selectChannel);
}

static void adc_startConvertion(void *channel)
{
    // printf("ADC startConvertion\r\n");
    REACT_PROFILER_COUNT(adc_startConvertion);
}

static bool adc_isConvertionReady(void *channel)
{
    // unsigned short *ch = (unsigned short *)channel;
    // printf("ADC isConvertionReady %d\r\n", *ch);

    REACT_PROFILER_COUNT(adc_isConvertionReady);
    return true;
}

static int16_t adc_readConvertion(void *channel)
{
    // unsigned short *ch = (unsigned short *)channel;
    // printf("ADC readConvertion %d\r\n", *ch);

    REACT_PROFILER_COUNT(adc_readConvertion);
    return sin(clock() / 1e6) * 100;
}


// UART
pthread_t uart_thread;
char      uart_received = 0;
void *    uart_receiver(void *ptr)
{
    while (true) {
        uart_received = fgetc(stdin);
    }
}
static void uart_init(void *baudrate)
{
    system("/bin/stty raw");
    pthread_create(&uart_thread, NULL, *uart_receiver, NULL);
    REACT_PROFILER_COUNT(uart_init);
}

static inline bool uart_isDataReceived()
{
    // printf("UART isDataReceived\r\n");
    REACT_PROFILER_COUNT(uart_isDataReceived);

    return uart_received != 0;
}

static inline bool uart_isTransmitReady()
{

    REACT_PROFILER_COUNT(uart_isTransmitReady);
    return true;
}

static inline void uart_transmit(unsigned char data)
{
    REACT_PROFILER_COUNT(uart_transmit);
    putchar(data);
}

static inline unsigned char uart_receive()
{
    char c        = uart_received;
    uart_received = 0;
    REACT_PROFILER_COUNT(uart_receive);
    return c;
}

/* Time */
static void timer_init(void *config)
{
    REACT_PROFILER_COUNT(timer_init);
    srand(time(NULL));
}

long          tick = 0;
unsigned long time_in_ns()
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1e9 + now.tv_nsec;
}

unsigned long timer_get_ns()
{
    long tock   = time_in_ns();
    long passed = tock - tick;

    if (tick) {
        tick = tock;
        return passed;
    }

    tick = tock;
    return 0;
}

static inline uint16_t timer_get()
{
    clock_t tick = clock();
    // printf("%ld - %ld\n", tick, timer_get_ns());
    REACT_PROFILER_COUNT_LOG(timer_get, "()=%d", tick);
    return tick;
    // printf("Timer get: %ld, %ld, %d\r\n", tick, CLOCKS_PER_SEC, tick /
    // CLOCKS_PER_SEC);
}

struct timer_callback {
    unsigned int timeout;
    void (*callback)(void *args);
    void *    args;
    size_t    index;
    pthread_t thread;
};

#define TIMERS_NR 10
struct timer_callback timer_callback_buffer[TIMERS_NR] = {0};

void *timer_timeout(void *ptr)
{
    struct timer_callback *callback = ptr;

    usleep(callback->timeout);
    callback->callback(callback->args);
    *callback = (struct timer_callback){0};

    return NULL;
}

static void timer_set(uint16_t ticks, void (*callback)(void *args), void *args)
{
    REACT_PROFILER_COUNT(timer_set);
    for (size_t i = 0; i < TIMERS_NR; i++) {
        if (timer_callback_buffer[i].callback == NULL) {
            struct timer_callback  timer_callback = {ticks, callback, args, i};
            struct timer_callback *timer          = timer_callback_buffer + i;
            *timer                                = timer_callback;
            pthread_create(&timer->thread, NULL, *timer_timeout, (void *)timer);

            // printf("Timer #%ld set: %d\r\n", i, ticks);
            break;
        }
    }
}

static void timer_off()
{
    REACT_PROFILER_COUNT(timer_off);
    // printf("Timer off\r\n");
}

static uint16_t timer_usFromTicks(uint16_t ticks)
{
    REACT_PROFILER_COUNT_LOG(timer_usFromTicks, "(%d)=%d", ticks, ticks);
    return ticks / 1;
}
