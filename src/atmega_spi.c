#include "stdint.h"
#include <avr/io.h>

#include "atmega_spi.h"

#define MOSI PORTB3
#define SCK PORTB5
#define SS PORTB2

void atmega_spi_master_init(void) {
  // set CS high
  PORTB |= (1 << SS);
  // Set MOSI, SCK, and SS output, all others input
  DDRB = (1 << MOSI) | (1 << SCK) | (1 << SS);
  // Enable SPI, saster, set clock rate fck/16
  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR1) | (1 << SPR0);
}

void atmega_spi_slave_init(void) {
  // Set MISO output, all others input
  DDRB = (1 << PINB4);
  // Enable SPI, slave, interrupt enable
  SPCR = (1 << SPE) | (1 << SPIE);
  // Preset SPI data register to 0
  SPDR = 0;
}

void atmega_spi_queue_transfer(uint8_t data) {
  SPDR = data;
}

uint8_t atmega_spi_transfer(uint8_t data) {
  // Start transmission
  atmega_spi_queue_transfer(data);
  // Wait for transmission complete
  while(!(SPSR & (1 << SPIF)));
  // Return data from MISO?
  return(SPDR);
}

uint8_t atmega_spi_receive(void) {
  // Wait for transmission complete
  while(!(SPSR & (1 << SPIF)));
  // Return data from MISO?
  return(SPDR);
}
