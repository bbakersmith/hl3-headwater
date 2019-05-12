/**
 * @file atmega_spi.h
 */

#ifndef _ATMEGA_SPI_H_
#define _ATMEGA_SPI_H_

#include "stdint.h"

/**
 * Initialize SPI and GPIO registers for master.
 */
void atmega_spi_master_init(void);

/**
 * Initialize SPI and GPIO registers for slave.
 */
void atmega_spi_slave_init(void);

/**
 * Update SPI write register. Master sends byte to slave(s) immediately,
 * slave sends on next write from master.
 */
void atmega_spi_queue_transfer(uint8_t data);

/**
 * Write one byte over SPI.
 */
uint8_t atmega_spi_transfer(uint8_t data);

/**
 * Read one byte from SPI.
 */
uint8_t atmega_spi_receive(void);

#endif
