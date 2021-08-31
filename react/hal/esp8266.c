#include "esp8266.h"

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

static void ICACHE_FLASH_ATTR gpio16_output_conf(void)
{
    WRITE_PERI_REG(
        PAD_XPD_DCDC_CONF,
        (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbcUL)
            | 0x1UL); // mux configuration for XPD_DCDC to output rtc_gpio0

    WRITE_PERI_REG(RTC_GPIO_CONF,
                   (READ_PERI_REG(RTC_GPIO_CONF) & 0xfffffffeUL)
                       | 0x0UL); // mux configuration for out enable

    WRITE_PERI_REG(RTC_GPIO_ENABLE,
                   (READ_PERI_REG(RTC_GPIO_ENABLE) & 0xfffffffeUL)
                       | 0x1UL); // out enable
}

static void ICACHE_FLASH_ATTR gpio16_input_conf(void)
{
    WRITE_PERI_REG(
        PAD_XPD_DCDC_CONF,
        (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbcUL)
            | 0x1UL); // mux configuration for XPD_DCDC and rtc_gpio0 connection

    WRITE_PERI_REG(RTC_GPIO_CONF,
                   (READ_PERI_REG(RTC_GPIO_CONF) & 0xfffffffeUL)
                       | 0x0UL); // mux configuration for out enable

    WRITE_PERI_REG(RTC_GPIO_ENABLE,
                   READ_PERI_REG(RTC_GPIO_ENABLE)
                       & 0xfffffffeUL); // out disable
}

/**
 * Returns the gpio name and func for a specific pin.
 */
bool ICACHE_FLASH_ATTR easygpio_getGPIONameFunc(uint8_t   gpio_pin,
                                                uint32_t *gpio_name,
                                                uint8_t * gpio_func)
{

    if (gpio_pin == 6 || gpio_pin == 7 || gpio_pin == 8 || gpio_pin == 11
        || gpio_pin >= 17) {
        os_printf("easygpio_getGPIONameFunc Error: There is no GPIO%d, check "
                  "your code\n",
                  gpio_pin);
        return false;
    }
    if (gpio_pin == 16) {
        os_printf("easygpio_getGPIONameFunc Error: GPIO16 does not have "
                  "gpio_name and gpio_func\n");
        return false;
    }
    switch (gpio_pin) {
    case 0:
        *gpio_func = FUNC_GPIO0;
        *gpio_name = PERIPHS_IO_MUX_GPIO0_U;
        return true;
    case 1:
        *gpio_func = FUNC_GPIO1;
        *gpio_name = PERIPHS_IO_MUX_U0TXD_U;
        return true;
    case 2:
        *gpio_func = FUNC_GPIO2;
        *gpio_name = PERIPHS_IO_MUX_GPIO2_U;
        return true;
    case 3:
        *gpio_func = FUNC_GPIO3;
        *gpio_name = PERIPHS_IO_MUX_U0RXD_U;
        return true;
    case 4:
        *gpio_func = FUNC_GPIO4;
        *gpio_name = PERIPHS_IO_MUX_GPIO4_U;
        return true;
    case 5:
        *gpio_func = FUNC_GPIO5;
        *gpio_name = PERIPHS_IO_MUX_GPIO5_U;
        return true;
    case 9:
        *gpio_func = FUNC_GPIO9;
        *gpio_name = PERIPHS_IO_MUX_SD_DATA2_U;
        return true;
    case 10:
        *gpio_func = FUNC_GPIO10;
        *gpio_name = PERIPHS_IO_MUX_SD_DATA3_U;
        return true;
    case 12:
        *gpio_func = FUNC_GPIO12;
        *gpio_name = PERIPHS_IO_MUX_MTDI_U;
        return true;
    case 13:
        *gpio_func = FUNC_GPIO13;
        *gpio_name = PERIPHS_IO_MUX_MTCK_U;
        return true;
    case 14:
        *gpio_func = FUNC_GPIO14;
        *gpio_name = PERIPHS_IO_MUX_MTMS_U;
        return true;
    case 15:
        *gpio_func = FUNC_GPIO15;
        *gpio_name = PERIPHS_IO_MUX_MTDO_U;
        return true;
    default:
        return false;
    }
    return true;
}

/**
 * Sets the pull up registers for a pin.
 */
static void ICACHE_FLASH_ATTR
easygpio_setupPullsByName(uint32_t gpio_name, EasyGPIO_PullStatus pullStatus)
{

    if (EASYGPIO_PULLUP == pullStatus) {
        PIN_PULLUP_EN(gpio_name);
    } else {
        PIN_PULLUP_DIS(gpio_name);
    }
}
/**
 * Sets the 'gpio_pin' pin as a GPIO and sets the pull register for that pin.
 * 'pullStatus' has no effect on output pins or GPIO16
 */
bool ICACHE_FLASH_ATTR easygpio_pinMode(uint8_t             gpio_pin,
                                        EasyGPIO_PullStatus pullStatus,
                                        EasyGPIO_PinMode    pinMode)
{
    uint32_t gpio_name;
    uint8_t  gpio_func;

    if (16 == gpio_pin) {
        // ignoring pull status on GPIO16 for now
        if (EASYGPIO_OUTPUT == pinMode) {
            gpio16_output_conf();
        } else {
            gpio16_input_conf();
        }
        return true;
    } else if (!easygpio_getGPIONameFunc(gpio_pin, &gpio_name, &gpio_func)) {
        return false;
    }

    PIN_FUNC_SELECT(gpio_name, gpio_func);
    easygpio_setupPullsByName(gpio_name, pullStatus);

    if (EASYGPIO_OUTPUT != pinMode) {
        GPIO_DIS_OUTPUT(GPIO_ID_PIN(gpio_pin));
    } else {
        // must enable the pin or else the WRITE_PERI_REG won't work
        gpio_output_set(0, 0, BIT(GPIO_ID_PIN(gpio_pin)), 0);
    }
    return true;
}

/**
 * Uniform way of setting GPIO output value. Handles GPIO 0-16.
 *
 * You can not rely on that this function will switch the gpio to an output like
 * GPIO_OUTPUT_SET does. Use easygpio_outputEnable() to change an input gpio to
 * output mode.
 */
void easygpio_outputSet(uint8_t gpio_pin, uint8_t value)
{
    if (16 == gpio_pin) {
        WRITE_PERI_REG(RTC_GPIO_OUT,
                       (READ_PERI_REG(RTC_GPIO_OUT) & 0xfffffffeUL)
                           | (0x1UL & value));
    } else {
#ifdef EASYGPIO_USE_GPIO_OUTPUT_SET
        GPIO_OUTPUT_SET(GPIO_ID_PIN(gpio_pin), value);
#else
        if (value & 1) {
            WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR,
                           READ_PERI_REG(PERIPHS_GPIO_BASEADDR)
                               | BIT(gpio_pin));
        } else {
            WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR,
                           READ_PERI_REG(PERIPHS_GPIO_BASEADDR)
                               & ~BIT(gpio_pin));
        }
#endif
    }
}

/**
 * Uniform way of getting GPIO input value. Handles GPIO 0-16.
 * The pin must be initiated with easygpio_pinMode() so that the pin mux is
 * setup as a gpio in the first place. If you know that you won't be using
 * GPIO16 then you'd better off by just using GPIO_INPUT_GET().
 */
uint8_t easygpio_inputGet(uint8_t gpio_pin)
{
    if (16 == gpio_pin) {
        return (READ_PERI_REG(RTC_GPIO_IN_DATA) & 1UL);
    } else {
#ifdef EASYGPIO_USE_GPIO_INPUT_GET
        return GPIO_INPUT_GET(GPIO_ID_PIN(gpio_pin));
#else
        // this does *not* work, maybe GPIO_IN_ADDRESS is the wrong address
        return ((GPIO_REG_READ(GPIO_IN_ADDRESS) > gpio_pin) & 1UL);
#endif
    }
}

static inline void in(void *pin)
{
    easygpio_pinMode(*(uint8_t *)pin, EASYGPIO_NOPULL, EASYGPIO_INPUT);
}

static inline void out(void *pin) {
    easygpio_pinMode(*(uint8_t *)pin, EASYGPIO_NOPULL, EASYGPIO_OUTPUT);
}


static inline void on(void *pin) {
    easygpio_outputSet(*(uint8_t *)pin, 1);
}

static inline void off(void *pin) {
    easygpio_outputSet(*(uint8_t *)pin, 0);
}

static inline void flip(void *pin) {}

static inline void pullup(void *pin) {
    easygpio_pinMode(*(uint8_t *)pin, EASYGPIO_PULLUP, EASYGPIO_INPUT);
}

static inline bool get(void *pin) { return easygpio_inputGet(*(uint8_t *)pin); }


// ADC

static void adc_mount(void *prescaler) {}

static inline void adc_selectChannel(void *channel) {}

static inline void adc_startConvertion(void *channel) {}

static inline bool adc_isConvertionReady(void *channel) { return true; }

static inline int adc_readConvertion(void *channel) { return false; }


// UART
static void uart_init(void *baudrate) {}

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


static inline bool uart_isDataReceived() { return true; }

static inline bool uart_isTransmitReady() { return true; }

static inline void uart_transmit(unsigned char data) {}

static inline unsigned char uart_receive() { return true; }


// SPI
static void spi_init(void *config) {}

static inline bool spi_isDataReceived() {}

static inline bool spi_isTransmitReady() {}

static inline void spi_transmit(unsigned char data) {}

static inline unsigned char spi_receive() {}


/* Time */
static void timer_init(void *config) {}

static inline unsigned int timer_get() {}

void (*_timer_func)(void *args);
void *_timer_func_args = 0;

void timer1_isr(void) { _timer_func(_timer_func_args); }

static void timer_set(unsigned int ticks, void (*callback)(void *args),
                      void *       args)
{
}

static void timer_off() {}

static unsigned int timer_usFromTicks(unsigned int ticks) { return ticks >> 1; }
