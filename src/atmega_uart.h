/**
 * @file atmega_uart.h
 */

#ifndef _ATMEGA_UART_H_
#define _ATMEGA_UART_H_

#include "stdbool.h"
#include "stdint.h"
#include <avr/io.h>

#include "bytes.h"

/**
 * Initialize UART registers.
 */
void atmega_uart_init(void);

/**
 * Write a data byte over UART.
 */
void atmega_uart_writer(uint8_t data);

/**
 * Check if UART is currently busy or ready for writing.
 *
 * @return True if ready for writing, false if busy
 */
bool atmega_uart_status_check(void);

#endif
