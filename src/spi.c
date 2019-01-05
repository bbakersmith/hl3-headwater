#include "stdint.h"

#include "headwater_state.h"
#include "spi.h"

/* struct LowHighBytes { */
/*   uint8_t low; */
/*   uint8_t high; */
/* }; */
/*  */
/* uint16_t spi_transfer_16bit(SPITransferFn transfer_fn, uint16_t data) { */
/*   uint8_t high = transfer_fn(spi_get_high_byte(data)); */
/*   uint8_t low = transfer_fn(spi_get_low_byte(data)); */
/*   return spi_high_low_to_16_bit(high, low); */
/* } */
/*  */
/* uint16_t spi_read_16bit(SPITransferFn transfer_fn) { */
/*   return spi_transfer_16bit(transfer_fn, 0); */
/* } */
/*  */
/* // Master can be synchronous */
/* uint16_t spi_master_read_samples_per_beat(SPITransferFn transfer_fn) { */
/*   transfer_fn(READ_SAMPLES_PER_BEAT); */
/*   return spi_read_16bit(transfer_fn); */
/* } */

// SPI from master will be initialized by a timed interrupt
// to avoid hammering slave with requests.
//
// Could achieve this by queuing up requests as needed, but
// probably more efficient to simply have the SPI interrupt
// check flags and build its own requests synchronously.

// initialize spi_client = SPISlaveClient{
//   .state = SPI_STATE_WRITE_LOW,
//   .spi_register = SPDR,
//   .callback_fn = &headwater_api_read_header
// }
uint8_t spi_get_low_byte(uint16_t data) {
  return data & 0xFF;
}

uint8_t spi_get_high_byte(uint16_t data) {
  return data >> 8;
}

uint16_t spi_high_low_to_16bit(uint8_t high, uint8_t low) {
  return (high << 8) | low;
}

/* void spi_slave_interrupt_handler(SPISlaveClient *client) { */
/*   switch(client.state) { */
/*     case SPI_STATE_WRITE_HIGH: */
/*       client->high = client.spi_register; */
/*       client->state = SPI_STATE_WRITE_LOW; */
/*       break; */
/*     case SPI_STATE_WRITE_LOW: */
/*       client->state = client.callback_fn(client.high, client.spi_register); */
/*       break; */
/*     case SPI_STATE_READ_HIGH: */
/*       client->spi_register = client.low; */
/*       client->state = SPI_STATE_READ_LOW; */
/*       break; */
/*     default: */
/*       client->state = SPI_STATE_WRITE_LOW; */
/*   } */
/* } */
/*  */
/* void spi_slave_read(SPISlaveClient *client, uint8_t data) { */
/*   client->spi_register = data; */
/*   client->state = SPI_STATE_READ_LOW; */
/* } */
/*  */
/* void spi_slave_read_16bit(SPISlaveClient *client, uint16_t data) { */
/*   client->low = spi_get_low_byte(data); */
/*   client->spi_register = spi_get_high_byte(data); */
/*   client->status = SPI_STATE_READ_HIGH; */
/* } */
/*  */
/* void spi_slave_write(SPISlaveClient *client, uint8_t *target) { */
/*   client.callback_fn = void (SPISlaveClient *spi_client) { */
/*     target = spi_client.low; */
/*   } */
/*   client->status = SPI_STATE_WRITE_LOW; */
/* } */
/*  */
/* void spi_slave_write_16bit(SPISlaveClient *client, uint16_t *target) { */
/*   client.callback_fn = void (uint8_t high, uint8_t low) { */
/*     target = spi_high_low_to_16bit(high, low); */
/*   } */
/*   client->status = SPI_STATE_WRITE_HIGH; */
/* } */
