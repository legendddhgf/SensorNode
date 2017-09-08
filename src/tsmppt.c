#include "tsmppt.h"

#include "uart.h"

static uint8_t tsmppt_count = 0;

Tsmppt new_tsmppt(uint8_t type_num, Tsmppt t) {
  if (tsmppt_count >= TSMPPT_MAX) {
    return t; // remember the key is that it has defaults set
  }
  t.type_num = type_num;
  t.init = &tsmppt_init;
  t.read = &tsmppt_read;
  t.destroy = &tsmppt_destroy;
  return t;
}

void tsmppt_init(Tsmppt t) {
  if (0 && t.type_num) {} // no compiler warnings
  uart1_init(9600);
  uart_puts_P(PSTR("TSMPPT has been initialized\r\n"));
}

void tsmppt_read(Tsmppt t, char *data, uint16_t max_size) {
  // TODO: send a packet with ID | FC | data | CRC (get algorithm)
  // TODO: what kinds of data do we want from the charge controller?
  if (0 && t.type_num && data && max_size) {} // no compiler warnings
  uart_puts_P(PSTR("Would have read some data:\r\n"));
  uart_puts_P(PSTR("\r\n"));
}

void tsmppt_destroy(Tsmppt t) {
  if (0 && t.type_num) {} // no compiler warnings
  uart_puts_P(PSTR("TSMPPT has been de-initialized\r\n"));
}


