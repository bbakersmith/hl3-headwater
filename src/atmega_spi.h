#include "stdint.h"

void atmega_spi_master_init(void);
void atmega_spi_slave_init(void);
void atmega_spi_queue_transfer(uint8_t data);
uint8_t atmega_spi_transfer(uint8_t data);
uint8_t atmega_spi_receive();
