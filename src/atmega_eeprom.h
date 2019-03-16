#ifndef _ATMEGA_EEPROM_H_
#define _ATMEGA_EEPROM_H_

#include "stdint.h"
#include <avr/io.h>

uint8_t atmega_eeprom_read(uint16_t address);
void atmega_eeprom_write(uint16_t address, uint8_t data);

#endif
