#include "stdint.h"
#include "spi.h"

struct LowHighBytes {
  uint8_t low;
  uint8_t high;
};

uint8_t spi_get_low_byte(uint16_t data) {
  return data & 0xFF;
}

uint8_t spi_get_high_byte(uint16_t data) {
  return data >> 8;
}

uint8_t spi_transfer_16bit(SPITransferFn transfer_fn, uint16_t data) {
  uint8_t high = transfer_fn(spi_get_high_byte(data));
  uint8_t low = transfer_fn(spi_get_low_byte(data));
  return (high << 8) | low;
}
