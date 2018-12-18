#include "stdint.h"

void spi_master_init(void);
void spi_slave_init(void);
uint8_t spi_queue_transfer(uint8_t data);
uint8_t spi_transfer(uint8_t data);
uint8_t spi_receive();
