#define F_CPU 16000000UL  // 16 MHz

#include <avr/io.h>
#include <util/delay.h>
#include "stdint.h"

#define LCD_PORT PORTD
#define LCD_DDR DDRD

#define LCD_RS_PIN PORTD6
#define LCD_E_PIN PORTD7

#define LCD_SR_PORT PORTB
#define LCD_SR_DDR DDRB

#define LCD_SR_CLK_PIN PORTB0
#define LCD_SR_DATA_PIN PORTB1

void atmega_lcd_init(void);
void atmega_lcd_write_cmd(uint8_t value);
void atmega_lcd_write_data(uint8_t value);
