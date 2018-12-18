#include "stdint.h"

typedef uint8_t (SPITransferFn)(uint8_t data);

uint8_t spi_get_high_byte(uint16_t data);
uint8_t spi_get_low_byte(uint16_t data);
uint8_t spi_transfer_16bit(SPITransferFn transfer_fn, uint16_t data);
