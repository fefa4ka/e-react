#include "MMA7455.h"
#include <circular.h>
#define MMA7455_SETTINGS bit(MMA7455_GLVL0) | bit(MMA7455_MODE0)


void is_settings_applied(char *message, int size, Component *trigger) {
    if(*message != MMA7455_SETTINGS)
        trigger->stage = prepared;
}

void is_status_readed(char *message, int size, Component *trigger) {
    trigger->stage = prepared; 
}

void MMA7455_write(char address, char data) {

  rb_write(NULL, (struct ring_node){ NULL, NULL });
}

willMount(MMA7455) {
  struct ring_node node = { MMA7455_MCTL };
  props->handler->write(ADDRESS, self, callback);

  spi_buffer_write(MMA7455_MCTL, MMA7455_SETTINGS, NULL, NULL);
  spi_buffer_read(MMA7455_MCTL, self, is_settings_applied);

  // Clear the offset registers.
  // If the Arduino was reset or with a warm-boot,
  // there still could be offset written in the sensor.
  // Only with power-up the offset values of the sensor
  // are zero.
  xyz.value.x = xyz.value.y = xyz.value.z = 0;
  error = MMA7455_write(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
}

shouldUpdate(MMA7455) {
 // Wait for status bit DRDY to indicate that
  // all 3 axis are valid.
  do
  {
    error = MMA7455_read (MMA7455_STATUS, &c, 1);
  } while ( !bitRead(c, MMA7455_DRDY) && error == 0);
}

willUpdate(MMA7455) { 
  // Read 6 bytes, containing the X,Y,Z information
  // as 10-bit signed integers.
  error = MMA7455_read (MMA7455_XOUTL, (uint8_t *) &xyz, 6);
  if (error != 0)
    return (error);

  self->stage = blocked;

}

release(MMA7455) {
  // The output is 10-bits and could be negative.
  // To use the output as a 16-bit signed integer,
  // the sign bit (bit 9) is extended for the 16 bits.
  if (xyz.reg.x_msb & 0x02)    // Bit 9 is sign bit.
    xyz.reg.x_msb |= 0xFC;     // Stretch bit 9 over other bits.
  else
    xyz.reg.x_msb &= 0x3;

  if (xyz.reg.y_msb & 0x02)
    xyz.reg.y_msb |= 0xFC;
  else
    xyz.reg.y_msb &= 0x3;

  if (xyz.reg.z_msb & 0x02)
    xyz.reg.z_msb |= 0xFC;
  else
    xyz.reg.z_msb &= 0x3;

  // The result is the g-force in units of 64 per 'g'.
  *pX = xyz.value.x;
  *pY = xyz.value.y;
  *pZ = xyz.value.z;
}

didMount(MMA7455) { }

didUnmount(MMA7455) { }
didUpdate(MMA7455) { }

React_Constructor(MMA7455)
