#include "atmega_eeprom.h"

uint8_t atmega_eeprom_read(uint16_t address) {
  // wait for any ongoing writes
  while(EECR & (1 << EEPE));

  EEAR = address;

  // start write
  EECR |= (1 << EERE);

  return EEDR;
}

void atmega_eeprom_write(uint16_t address, uint8_t data) {
  if(data != atmega_eeprom_read(address)) {
    EEAR = address;
    EEDR = data;

    // enable write
    EECR |= (1 << EEMPE);

    // start write
    EECR |= (1 << EEPE);
  }
}
