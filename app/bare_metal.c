// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */


int main(void) {

  // -------- Inits --------- //
  DDRB |= 0b00000010;            /* Data Direction Register B:
                                   writing a one to the bit
                                   enables output. */

  // ------ Event loop ------ //
  while (1) {

    PORTB = 0b00000010;          /* Turn on first LED bit/pin in PORTB */

    PORTB = 0b00000000;          /* Turn off all B pins, including LED */

  }                                                  /* End event loop */
  return 0;                            /* This line is never reached */
}
