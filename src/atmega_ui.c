#include "stdint.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>

#define OUTPUT_PORT PORTB
#define OUTPUT_DDR DDRB

#define LCD_SR_CLK PORTB0
#define LCD_SR_DATA PORTB1

void main(void) {
  OUTPUT_DDR |= (1 << LCD_SR_CLK) | (1 << LCD_SR_DATA);

  // start clock low
  OUTPUT_PORT &= ~(1 << LCD_SR_CLK);

  while(1) {
    // set high
    OUTPUT_PORT |= (1 << LCD_SR_DATA);
    _delay_ms(100);

    // clock 1
    OUTPUT_PORT |= (1 << LCD_SR_CLK);
    _delay_ms(100);
    OUTPUT_PORT &= ~(1 << LCD_SR_CLK);
    _delay_ms(100);

    // set low
    OUTPUT_PORT &= ~(1 << LCD_SR_DATA);
    _delay_ms(100);

    // clock 2
    OUTPUT_PORT |= (1 << LCD_SR_CLK);
    _delay_ms(100);
    OUTPUT_PORT &= ~(1 << LCD_SR_CLK);
    _delay_ms(100);

    // set high
    OUTPUT_PORT |= (1 << LCD_SR_DATA);
    _delay_ms(100);

    // clock 3
    OUTPUT_PORT |= (1 << LCD_SR_CLK);
    _delay_ms(100);
    OUTPUT_PORT &= ~(1 << LCD_SR_CLK);
    _delay_ms(100);

    // set high
    OUTPUT_PORT |= (1 << LCD_SR_DATA);
    _delay_ms(100);

    // clock 4
    OUTPUT_PORT |= (1 << LCD_SR_CLK);
    _delay_ms(100);
    OUTPUT_PORT &= ~(1 << LCD_SR_CLK);
    _delay_ms(100);

    // set low
    OUTPUT_PORT &= ~(1 << LCD_SR_DATA);
    _delay_ms(100);

    // clock 5
    OUTPUT_PORT |= (1 << LCD_SR_CLK);
    _delay_ms(100);
    OUTPUT_PORT &= ~(1 << LCD_SR_CLK);
    _delay_ms(100);

    // set low
    OUTPUT_PORT &= ~(1 << LCD_SR_DATA);
    _delay_ms(100);

    // clock 6
    OUTPUT_PORT |= (1 << LCD_SR_CLK);
    _delay_ms(100);
    OUTPUT_PORT &= ~(1 << LCD_SR_CLK);
    _delay_ms(100);

    // set high
    OUTPUT_PORT |= (1 << LCD_SR_DATA);
    _delay_ms(100);

    // clock 7
    OUTPUT_PORT |= (1 << LCD_SR_CLK);
    _delay_ms(100);
    OUTPUT_PORT &= ~(1 << LCD_SR_CLK);
    _delay_ms(100);

    // set low
    OUTPUT_PORT &= ~(1 << LCD_SR_DATA);
    _delay_ms(100);

    // clock 8
    OUTPUT_PORT |= (1 << LCD_SR_CLK);
    _delay_ms(100);
    OUTPUT_PORT &= ~(1 << LCD_SR_CLK);
    _delay_ms(100);
  }
}
