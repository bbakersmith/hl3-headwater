#include "stdint.h"

#include "bytes.h"

uint8_t bytes_16bit_to_low(uint16_t data) {
  return data & 0xFF;
}

uint8_t bytes_16bit_to_high(uint16_t data) {
  return data >> 8;
}

uint16_t bytes_high_low_to_16bit(uint8_t high, uint8_t low) {
  return (high << 8) | low;
}
