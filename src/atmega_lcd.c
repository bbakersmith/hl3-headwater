#include "atmega_lcd.h"

void atmega_lcd_init(void) {
  // enable lcd pins
  LCD_DDR |= (1 << LCD_RS_PIN) | (1 << LCD_E_PIN);

  // enable lcd shift register pins
  LCD_SR_DDR |= (1 << LCD_SR_CLK_PIN) | (1 << LCD_SR_DATA_PIN);

  // wait for LCD
  _delay_ms(500);

  // turn on display and cursor
  atmega_lcd_send_cmd(0x0E);
  _delay_ms(5);

  // clear display
  atmega_lcd_send_cmd(0x01);
  _delay_ms(5);

  // function set
  atmega_lcd_send_cmd(0x3C);
  _delay_ms(5);

  // move to start
  atmega_lcd_send_cmd(0x80);
  _delay_ms(5);
}

void atmega_lcd_74164_send(uint8_t data) {
  for(uint8_t i = 0; i < 8; i++) {
    if(((data >> i) & 1) == 0) {
      LCD_SR_PORT &= ~(1 << LCD_SR_DATA_PIN);
    } else {
      LCD_SR_PORT |= (1 << LCD_SR_DATA_PIN);
    }
    LCD_SR_PORT |= (1 << LCD_SR_CLK_PIN);
    LCD_SR_PORT &= ~(1 << LCD_SR_CLK_PIN);
  }
}

void atmega_lcd_execute(void) {
  LCD_PORT |= (1 << LCD_E_PIN);
  LCD_PORT &= ~(1 << LCD_E_PIN);
}

void atmega_lcd_send(bool rs, uint8_t data) {
  if(rs == 0) {
    LCD_PORT &= ~(1 << LCD_RS_PIN);
  } else {
    LCD_PORT |= (1 << LCD_RS_PIN);
  }
  atmega_lcd_74164_send(data);
  atmega_lcd_execute();
}

void atmega_lcd_send_cmd(uint8_t data) {
  atmega_lcd_send(0, data);
}

void atmega_lcd_send_data(uint8_t data) {
  atmega_lcd_send(1, data);
}
