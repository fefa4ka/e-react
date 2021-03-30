#include "MMA7455.h"
#include <SPI.h>
#include <circular.h>
#define MMA7455_SETTINGS bit (MMA7455_GLVL0) | bit (MMA7455_MODE0)


#define write(address, variable)                                      \
    SPI_write (props->spi, address, variable, props->chip_select_pin);

#define read(address, callback_method)                                       \
    {                                                                         \
        struct callback method = { .method = callback_method, .argument = self };  \
        SPI_read (props->spi, address, &method, props->chip_select_pin);    \
    }

#define read_value(address, variable)                                 \
    {                                                                         \
        struct callback callback                                              \
            = { .method = MMA7455_read_protocol_value, .argument = &variable };       \
        SPI_read (props->spi, address, &callback, props->chip_select_pin);    \
    }

void
MMA7455_read_protocol_value (void *value, void *variable)
{
    *(unsigned char *)variable = *(unsigned char *)value;
}


static void
isAxisDataReady (void *message, void *trigger)
{
    Component *         self = (Component *)trigger;
    MMA7455_state_t *state  = (MMA7455_state_t *)self->state;
    if (bit_value (*(unsigned char *)message, MMA7455_DRDY)) {
        state->ready = true;
    } else {
        state->ready = false;
    }
}

static void
axisDataReaded (void *message, void *trigger)
{
    Component *self = (Component *)trigger;
    self->stage     = REACT_STAGE_PREPARED;
}

static void
isModeSynced (void *message, void *trigger)
{
    Component *         self   = (Component *)trigger;
    MMA7455_props_t *props  = (MMA7455_props_t *)self->props;

    unsigned char  mode     = *(unsigned char *)message;
    unsigned short selected = 0;

    self->stage = REACT_STAGE_RELEASED;

    switch (mode & MMA7455_MCTL_GLVL_MASK) {
    case MMA7455_MCTL_GLVL_2G:
        selected = 2;
        break;
    case MMA7455_MCTL_GLVL_4G:
        selected = 4;
        break;
    case MMA7455_MCTL_GLVL_8G:
        selected = 8;
        break;
    default:
        selected = 0;
        break;
    }
    if (props->sensitivity != selected) {
        self->stage = REACT_STAGE_DEFINED;
    }

    switch (mode & MMA7455_MCTL_MOD_MASK) {
    case MMA7455_MCTL_MOD_STBY:
        selected = MMA7455_MODE_STANDBY;
        break;
    case MMA7455_MCTL_MOD_MSMT:
        selected = MMA7455_MODE_MEASURE;
        break;
    case MMA7455_MCTL_MOD_LVL:
        selected = MMA7455_MODE_LEVEL;
        break;
    case MMA7455_MCTL_MOD_PLS:
        selected = MMA7455_MODE_PULSE;
        break;
    default:
        selected = MMA7455_MODE_NONE;
        break;
    }
    if (props->mode != selected) {
        self->stage = REACT_STAGE_DEFINED;
    }
}

static void
setMode (void *message, void *trigger)
{
    Component *         self     = (Component *)trigger;
    MMA7455_props_t *props    = (MMA7455_props_t *)self->props;
    unsigned char       mode     = *(unsigned char *)message;
    unsigned short      selected = 0;

    switch (props->sensitivity) {
    case 2:
        selected = MMA7455_MCTL_GLVL_2G;
        break;
    case 4:
        selected = MMA7455_MCTL_GLVL_4G;
        break;
    case 8:
        selected = MMA7455_MCTL_GLVL_8G;
        break;
    default:
        selected = MMA7455_MCTL_GLVL_2G;
        break;
    }
    mode &= ~MMA7455_MCTL_GLVL_MASK;
    mode |= selected & MMA7455_MCTL_GLVL_MASK;

    switch (props->mode) {
    case MMA7455_MODE_STANDBY:
        selected = MMA7455_MCTL_MOD_STBY;
        break;
    case MMA7455_MODE_MEASURE:
        selected = MMA7455_MCTL_MOD_MSMT;
        break;
    case MMA7455_MODE_LEVEL:
        selected = MMA7455_MCTL_MOD_LVL;
        break;
    case MMA7455_MODE_PULSE:
        selected = MMA7455_MCTL_MOD_PLS;
        break;
    default:
        selected = MMA7455_MCTL_MOD_MSMT;
        break;
    }
    mode &= ~MMA7455_MCTL_MOD_MASK;
    mode |= selected & MMA7455_MCTL_MOD_MASK;

    write (MMA7455_MCTL, mode);
    read (MMA7455_MCTL, isModeSynced);
}

willMount (MMA7455)
{
    // Clear the offset registers.
    // If the Arduino was reset or with a warm-boot,
    // there still could be offset written in the sensor.
    // Only with power-up the offset values of the sensor
    // are zero.
    state->force.value.x = state->force.value.y = state->force.value.z = 0;
    state->ready                                                       = false;
    write (MMA7455_XOFFL, 0x00);
    write (MMA7455_XOFFH, 0x00);
    write (MMA7455_YOFFL, 0x00);
    write (MMA7455_YOFFH, 0x00);
    write (MMA7455_ZOFFL, 0x00);
    write (MMA7455_ZOFFH, 0x00);
    write (MMA7455_MCTL, 0x00);
    write (MMA7455_INTRST, 0x03);
    write (MMA7455_INTRST, 0x00);
    write (MMA7455_CTL1, 0x00);
    write (MMA7455_CTL2, 0x00);
    write (MMA7455_LDTH, 0x00);
    write (MMA7455_PDTH, 0x00);
    //write (MMA7455_PW, 0x00);
    write (MMA7455_LT, 0x00);
    write (MMA7455_TW, 0x00);

    read (MMA7455_MCTL, setMode);
}

shouldUpdate (MMA7455)
{
    if (props->sensitivity != next_props->sensitivity
        || props->mode != next_props->mode) {
        return true;
    }

    if (state->ready) {
        return true;
    }

    // Wait for status bit DRDY to indicate that
    // all 3 axis are valid.
    read (MMA7455_STATUS, isAxisDataReady);

    return false;
}

willUpdate (MMA7455)
{
    if (props->sensitivity != next_props->sensitivity
        || props->mode != next_props->mode) {
        read (MMA7455_MCTL, setMode);
    } else {
        // Read 6 bytes, containing the X,Y,Z information
        // as 10-bit signed integers.
        read_value (MMA7455_XOUTL, state->force.reg.x_lsb);
        read_value (MMA7455_XOUTH, state->force.reg.x_msb);

        read_value (MMA7455_YOUTL, state->force.reg.y_lsb);
        read_value (MMA7455_YOUTH, state->force.reg.y_msb);

        read_value (MMA7455_ZOUTL, state->force.reg.y_lsb);
        read_value (MMA7455_ZOUTH, state->force.reg.y_msb);
        read (MMA7455_STATUS, axisDataReaded);
    }

    self->stage = REACT_STAGE_BLOCKED;
}

release (MMA7455)
{
    // The output is 10-bits and could be negative.
    // To use the output as a 16-bit signed integer,
    // the sign bit (bit 9) is extended for the 16 bits.
    if (state->force.reg.x_msb & 0x02)  // Bit 9 is sign bit.
        state->force.reg.x_msb |= 0xFC; // Stretch bit 9 over other bits.
    else
        state->force.reg.x_msb &= 0x3;

    if (state->force.reg.y_msb & 0x02)
        state->force.reg.y_msb |= 0xFC;
    else
        state->force.reg.y_msb &= 0x3;

    if (state->force.reg.z_msb & 0x02)
        state->force.reg.z_msb |= 0xFC;
    else
        state->force.reg.z_msb &= 0x3;

    // The result is the g-force in units of 64 per 'g'.
    // in state->force.value
    if(props->onChange)
        props->onChange(self);
}

didMount (MMA7455) {}

didUnmount (MMA7455) {}
didUpdate (MMA7455) { state->ready = false; }

React_Constructor (MMA7455)
