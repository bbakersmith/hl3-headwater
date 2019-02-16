#include "headwater_lcd.h"

void headwater_lcd_update_main_bpm(LCD *lcd, uint16_t value) {
  uint8_t digit;

  digit = value % 10;
  lcd->characters[4] = lcd_digit_to_char(digit);
  value /= 10;

  lcd->characters[3] = LCD__DOT;

  digit = value % 10;
  lcd->characters[2] = lcd_digit_to_char(digit);
  value /= 10;

  if(0 < value) {
    digit = value % 10;
    lcd->characters[1] = lcd_digit_to_char(digit);
    value /= 10;
  } else {
    lcd->characters[1] = LCD__;
  }

  if(0 < value) {
    digit = value % 10;
    lcd->characters[0] = lcd_digit_to_char(digit);
  } else {
    lcd->characters[0] = LCD__;
  }
}

void headwater_lcd_update_main_multiplier_a(LCD *lcd, uint16_t value) {
  uint8_t digit;

  digit = value % 10;
  lcd->characters[12] = lcd_digit_to_char(digit);
  value /= 10;

  if(0 < value) {
    digit = value % 10;
    lcd->characters[11] = lcd_digit_to_char(digit);
    value /= 10;
  } else {
    lcd->characters[0] = LCD__;
  }
}

void headwater_lcd_update_main_multiplier_b(LCD *lcd, uint16_t value) {
  uint8_t digit;

  digit = value % 10;
  lcd->characters[15] = lcd_digit_to_char(digit);
  value /= 10;

  if(0 < value) {
    digit = value % 10;
    lcd->characters[14] = lcd_digit_to_char(digit);
    value /= 10;
  } else {
    lcd->characters[0] = LCD__;
  }
}

void headwater_lcd_update_main(LCD *lcd, HeadwaterState *state) {
  headwater_lcd_update_main_bpm(lcd, state->bpm);

  // TODO should be able to change units to ms, hz
  lcd->characters[6] = LCD__B;
  lcd->characters[7] = LCD__P;
  lcd->characters[8] = LCD__M;

  // TODO preset
  lcd->characters[27] = LCD__9;
  lcd->characters[28] = LCD__9;

  // TODO next preset
  lcd->characters[30] = LCD__9;
  lcd->characters[31] = LCD__9;

  // TODO other modes, use enum
  if(state->mode == HEADWATER_STATE_MODE_INT) {
    lcd->characters[22] = LCD__I;
    lcd->characters[23] = LCD__N;
    lcd->characters[24] = LCD__T;
    lcd->characters[25] = LCD__;
  }

  headwater_lcd_update_main_multiplier_a(
    lcd,
    state->multiplier_a_channel.multiplier
  );

  headwater_lcd_update_main_multiplier_b(
    lcd,
    state->multiplier_b_channel.multiplier
  );

  if(state->output_enabled == 1) {
    lcd->characters[16] = LCD__P;
    lcd->characters[17] = LCD__L;
    lcd->characters[18] = LCD__A;
    lcd->characters[19] = LCD__Y;
  } else {
    lcd->characters[16] = LCD__S;
    lcd->characters[17] = LCD__T;
    lcd->characters[18] = LCD__O;
    lcd->characters[19] = LCD__P;
  }
}
