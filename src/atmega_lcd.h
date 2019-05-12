/**
 * @file atmega_lcd.h
 *
 * GPIO setup and write functions for updating 16x02 style LCD display.
 */

#ifndef _ATMEGA_LCD_H_
#define _ATMEGA_LCD_H_

#define F_CPU 16000000UL  // 16 MHz

#include "stdbool.h"
#include "stdint.h"
#include <avr/io.h>
#include <util/delay.h>

#define LCD_PORT PORTD
#define LCD_DDR DDRD

#define LCD_RS_PIN PORTD6
#define LCD_E_PIN PORTD7

#define LCD_SR_PORT PORTB
#define LCD_SR_DDR DDRB

#define LCD_SR_CLK_PIN PORTB0
#define LCD_SR_DATA_PIN PORTB1

/**
 * Initialize GPIO for outputs to LCD.
 */
void atmega_lcd_init(void);

/**
 * Send data to LCD.
 *
 * @param State of RS pin
 * @param Data byte
 */
void atmega_lcd_send(bool rs, uint8_t data);

/**
 * Send command to LCD.
 *
 * @param Command byte
 */
void atmega_lcd_send_cmd(uint8_t data);

/**
 * Send data to LCD.
 *
 * @param Data byte
 */
void atmega_lcd_send_data(uint8_t data);

#endif
