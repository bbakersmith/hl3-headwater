#ifndef _ATMEGA_UART_H_
#define _ATMEGA_UART_H_

#include "stdint.h"
#include <avr/io.h>

#include "bytes.h"

void atmega_uart_init(void);
void atmega_uart_writer(uint8_t data);
uint8_t atmega_uart_status_check(void);

#endif
