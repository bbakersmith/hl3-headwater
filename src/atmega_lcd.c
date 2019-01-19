#include "atmega_lcd.h"

void atmega_lcd_init(void) {
  // enable lcd pins
  LCD_DDR |= (1 << LCD_RS_PIN) | (1 << LCD_E_PIN);

  // enable lcd shift register pins
  LCD_SR_DDR |= (1 << LCD_SR_CLK_PIN) | (1 << LCD_SR_DATA_PIN);
}

void atmega_lcd_74164_write(uint8_t value) {
  for(uint8_t i; i < 8; i++) {
    if(((value >> i) & 1) == 0) {
      LCD_SR_PORT &= ~(1 << LCD_SR_DATA_PIN);
    } else {
      LCD_SR_PORT |= (1 << LCD_SR_DATA_PIN);
    }
    LCD_SR_PORT |= (1 << LCD_SR_CLK_PIN);
    LCD_SR_PORT &= ~(1 << LCD_SR_CLK_PIN);
  }
}

void atmega_lcd_update(void) {
  // TODO disabling E should be part of interrupt
  LCD_PORT |= (1 << LCD_E_PIN);
  _delay_ms(2);
  LCD_PORT &= ~(1 << LCD_E_PIN);
  _delay_ms(2);
}

void atmega_lcd_write(uint8_t value) {
  atmega_lcd_74164_write(value);
  atmega_lcd_update();
}

void atmega_lcd_write_cmd(uint8_t value) {
  LCD_PORT &= ~(1 << LCD_RS_PIN);
  atmega_lcd_write(value);
}

void atmega_lcd_write_data(uint8_t value) {
  LCD_PORT |= (1 << LCD_RS_PIN);
  atmega_lcd_write(value);
}
