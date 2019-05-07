#include "atmega_uart.h"

#define ATMEGA_UART_BAUDRATE 31250
#define ATMEGA_UART_PRESCALE (((16000000 / (ATMEGA_UART_BAUDRATE * 16UL))) - 1)

void atmega_uart_init(void) {
  /* set baud rate */
  UBRR0H = bytes_16bit_to_high(ATMEGA_UART_PRESCALE);
  UBRR0L = bytes_16bit_to_low(ATMEGA_UART_PRESCALE);

  /* enable reader, transmitter, and associated interrupts */
  UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);

  /* Set frame format: 8data, 1 stop bit */
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void atmega_uart_writer(uint8_t data) {
  UDR0 = data;
}

uint8_t atmega_uart_status_check(void) {
  return (UCSR0A & (1 << UDRE0)) == 0;
}
