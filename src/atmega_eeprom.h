/**
 * @file atmega_eeprom.h
 *
 * Read and write EEPROM on Atmega328p.
 */

#ifndef _ATMEGA_EEPROM_H_
#define _ATMEGA_EEPROM_H_

#include "stdint.h"
#include <avr/io.h>

/**
 * Read data byte at given address.
 */
uint8_t atmega_eeprom_read(uint16_t address);

/**
 * Write data byte to given address.
 */
void atmega_eeprom_write(uint16_t address, uint8_t data);

#endif
