#ifndef _BYTES_H_
#define _BYTES_H_

#include "stdint.h"

uint8_t bytes_16bit_to_low(uint16_t data);
uint8_t bytes_16bit_to_high(uint16_t data);
uint16_t bytes_high_low_to_16bit(uint8_t high, uint8_t low);

#endif
