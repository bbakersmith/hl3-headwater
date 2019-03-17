#include "headwater_lcd.h"

// TODO this should be in headwater_ui?
// TODO character positions should be relative to select position

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

void headwater_lcd_update_main_preset(LCD *lcd, uint8_t value) {
  uint8_t digit;

  digit = value % 10;
  lcd->characters[23] = lcd_digit_to_char(digit);
  value /= 10;

  if(0 < value) {
    digit = value % 10;
    lcd->characters[22] = lcd_digit_to_char(digit);
    value /= 10;
  } else {
    lcd->characters[22] = LCD__;
  }
}

void headwater_lcd_update_main_multiplier_a(LCD *lcd, uint16_t value) {
  uint8_t digit;

  digit = value % 10;
  lcd->characters[17] = lcd_digit_to_char(digit);
  value /= 10;

  if(0 < value) {
    digit = value % 10;
    lcd->characters[16] = lcd_digit_to_char(digit);
    value /= 10;
  } else {
    lcd->characters[16] = LCD__;
  }
}

void headwater_lcd_update_main_multiplier_b(LCD *lcd, uint16_t value) {
  uint8_t digit;

  digit = value % 10;
  lcd->characters[20] = lcd_digit_to_char(digit);
  value /= 10;

  if(0 < value) {
    digit = value % 10;
    lcd->characters[19] = lcd_digit_to_char(digit);
    value /= 10;
  } else {
    lcd->characters[19] = LCD__;
  }
}

void headwater_lcd_update_main_mode(LCD *lcd, uint8_t value) {
  uint8_t char1;
  uint8_t char2;

  switch(value) {
    case HEADWATER_STATE_MODE_INTERNAL:
      char1 = LCD__I;
      char2 = LCD__N;
      break;
    case HEADWATER_STATE_MODE_TAP:
      char1 = LCD__T;
      char2 = LCD__P;
      break;
    case HEADWATER_STATE_MODE_EXTERNAL:
      char1 = LCD__E;
      char2 = LCD__X;
      break;
    case HEADWATER_STATE_MODE_MIDI:
      char1 = LCD__M;
      char2 = LCD__I;
      break;
    default:
      char1 = LCD__QUES;
      char2 = LCD__QUES;
  }

  lcd->characters[6] = char1;
  lcd->characters[7] = char2;
}
