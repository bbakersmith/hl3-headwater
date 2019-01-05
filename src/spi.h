#include "stdint.h"

enum SPI_STATE {
  SPI_STATE_READ_HIGH,
  SPI_STATE_READ_LOW,
  SPI_STATE_WRITE_HIGH,
  SPI_STATE_WRITE_LOW
};

// Callback function returns new state for client
// typedef enum SPI_STATE (SPISlaveCallbackFn)(uint8_t high, uint8_t low);

struct SPISlaveClient {
  uint8_t high;
  uint8_t low;
  uint8_t spi_register;
  enum SPI_STATE state;
  void (*callback_fn)(struct SPISlaveClient *spi_client);
} SPISlaveClient;

uint8_t spi_get_high_byte(uint16_t data);
uint8_t spi_get_low_byte(uint16_t data);
