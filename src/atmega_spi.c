#include "stdint.h"
#include <avr/io.h>

#include "atmega_spi.h"

#define SPI_PORT PORTB
#define SPI_DDR DDRB

#define SS_PIN PORTB2
#define MOSI_PIN PORTB3
#define MISO_PIN PORTB4
#define SCK_PIN PORTB5

void atmega_spi_queue_transfer(uint8_t data) {
  SPDR = data;
}

void atmega_spi_wait_for_transfer(void) {
  while(!(SPSR & (1 << SPIF)));
}

uint8_t atmega_spi_transfer(uint8_t data) {
  // Start transmission
  atmega_spi_queue_transfer(data);
  // Wait for transmission complete
  atmega_spi_wait_for_transfer();
  // Return data from MISO?
  return(SPDR);
}

uint8_t atmega_spi_receive(void) {
  // Wait for transmission complete
  atmega_spi_wait_for_transfer();
  // Return data from MISO?
  return(SPDR);
}

void atmega_spi_master_init(void) {
  // set CS high
  SPI_PORT |= (1 << SS_PIN);
  // Set MOSI, SCK, and SS output, all others input
  SPI_DDR = (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);
  // Enable SPI, saster, set clock rate fck/16
  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR1) | (1 << SPR0);
}

void atmega_spi_slave_init(void) {
  // Set MISO output, all others input
  SPI_DDR |= (1 << MISO_PIN);
  // Enable SPI, slave, interrupt enable
  SPCR |= (1 << SPE) | (1 << SPIE);
  // Preset SPI data register to 0
  atmega_spi_queue_transfer(0);
}
