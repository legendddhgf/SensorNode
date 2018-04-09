#include "tsmppt.h"

#include "uart.h"

// TODO: configure this modbus ID for the charge controller
#define DEVICE_ADDRESS 0xA // 2 hex chars

#define FC_READ_HOLDING_REG 0x3 // 2 hex

//data field is 8 bits as specified by tristar modbus document
#define VOLTAGE_SCALE_ADDRESS 0x1 // 4 hex chars
#define BATTERY_VOLTAGE_ADDRESS_FILTER 0x18 // 4 hex chars (Slow Filter)
#define BATTERY_VOLTAGE_ADDRESS_TERMINAL 0x12 //4 hex chars (Terminal)
#define ONEBYTE 1
// returns 16 CRC with low byte then high byte
uint16_t CRC16(uint8_t *buf, uint16_t len)
{
  uint16_t crc = 0xFFFF;

  for (uint16_t pos = 0; pos < len; pos++)
  {
    crc ^= buf[pos];          // XOR byte into least sig. byte of crc

    for (uint8_t i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped
  return crc;
}

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

  char bufQuery[256] = {};
  char bufResponse[256] = {};
  // SEND PACKET REQUESTING HIGH BYTE OF VOLTAGE SCALE
  _delay_ms(4); // delay time for approximately 4 characters at this setting
  sprintf(bufQuery, "%02X%02X%04X%04X", DEVICE_ADDRESS, FC_READ_HOLDING_REG,
      VOLTAGE_SCALE_ADDRESS, ONEBYTE);
  uint16_t crc = CRC16((uint8_t *) bufQuery, 8); // 8 hex digits up till now (remember reversed)
  // place the crc as two bytes at the end of message (already low, hi order)
  sprintf(bufQuery, "%X%X%X%X%X%X", DEVICE_ADDRESS, FC_READ_HOLDING_REG,
      VOLTAGE_SCALE_ADDRESS, ONEBYTE, (crc >> 8) & 0xFF, crc & 0xFF);
  uart_printf("Packet before take-off: %s\r\n",bufQuery);
  uart_printf("Device Address: %X, Read cmd: %X, Scaler Address: %X,  onebyte:%X crchi: %X, crclow: %X\r\n", DEVICE_ADDRESS, FC_READ_HOLDING_REG,
    VOLTAGE_SCALE_ADDRESS, ONEBYTE,(crc >> 8) & 0xFF, crc & 0xFF );
  uart1_printf(bufQuery);

  // GET PACKET WITH HIGH BYTE OF VOLTAGE SCALE
  _delay_ms(4); // delay time for approximately 4 characters at this setting
  for (uint8_t i = 0; i < 255; i++) {
    bufResponse[i] = (char) uart1_getc();
    if (bufResponse[i] == '\0') {
      uart_puts_P(PSTR("Unable to read expected 12 bytes of data\r\n"));
      snprintf(data, max_size, "%d", (uint8_t) ~0); // copy data to be sent off
      return;
    }
    uart_printf("Got byte from morningstar with value: %d\n", (int8_t)
        bufResponse[i]);
  }
  bufQuery[255] = '\0'; // end string

  uart_puts_P(PSTR("Read some data:\r\n"));
  uart_printf("size = %zd: %s\r\n", strlen(bufResponse), bufResponse);
  uart_puts_P(PSTR("\r\n"));
  snprintf(data, max_size, bufResponse); // copy data to be sent off
}

void tsmppt_destroy(Tsmppt t) {
  if (0 && t.type_num) {} // no compiler warnings
  uart_puts_P(PSTR("TSMPPT has been de-initialized\r\n"));
}

void get_packet(char *buf, uint16_t regHi, uint16_t regLo, uint8_t numRegHi,
    uint8_t numRegLo){
  // request packet
  _delay_ms(4);
  sprintf(buf, "%02X%02X%04X%04X%02X%02X", DEVICE_ADDRESS,
      FC_READ_HOLDING_REG, regHi, regLo, numRegHi, numRegLo);
  uint16_t crc = CRC16((uint8_t *) buf, 8);
  sprintf(buf, "%02X%02X%04X%04X%02X%02X%04X", DEVICE_ADDRESS, FC_READ_HOLDING_REG,
      regHi, regLo, numRegHi, numRegLo, crc);
  uart1_printf(buf);
  //receive packet
  _delay_ms(4);
  for(uint8_t i = 0; i < 12; i++){
    buf[i] = (char) uart1_getc();
    if(buf[i] == '\0'){
      uart_puts_P(PSTR("Unable to read expected 12 bytes of data\r\n"));
      return;
    }
    uart_printf("Got byte from morningstar with value: %d\n", (int8_t) buf[i]);
  }
  buf[12] = '\0';
  uart_puts_P(PSTR("Successfully read some data: \r\n"));
  uart_printf("size = %zd: %s\r\n", strlen(buf), buf);
  uart_puts_P(PSTR("\r\n"));
}
/*
void get_battery_voltage(uint8_t *data){
  
}

//Vscaling = whole.fraction = [V_PU hi] . [V_PU lo]
// addresses 1,2 [0x0000, 0x0001]
// only needs to be computed once
void calc_volt_scaler(uint8_t *scaler){
    char bufHi[256];
    char bufLo[256];
    get_packet(bufHi);
    get_packet(bufLo);
    uint8_t whole_term = bufHigh;
    uint8_t fractional_term = bufLow >> 16; divided by 16
    scaler= whole_term + fractional_term;
}

void calc_current_scaler(uint8_t *scaler){

}
*/
