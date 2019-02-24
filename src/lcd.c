#include "lcd.h"

LCD_CHAR lcd_digit_to_char(uint8_t digit) {
  LCD_CHAR character;

  switch(digit) {
    case 0:
      character = LCD__0;
      break;
    case 1:
      character = LCD__1;
      break;
    case 2:
      character = LCD__2;
      break;
    case 3:
      character = LCD__3;
      break;
    case 4:
      character = LCD__4;
      break;
    case 5:
      character = LCD__5;
      break;
    case 6:
      character = LCD__6;
      break;
    case 7:
      character = LCD__7;
      break;
    case 8:
      character = LCD__8;
      break;
    case 9:
      character = LCD__9;
      break;
    default:
      character = LCD__;
  }

  return character;
}

LCD lcd_new(void) {
  LCD lcd = {
    .characters_index = 0,
    .mode = LCD_MODE_WRITE,
    .rows_index = -1,
    .selected_position = 0x79, // FIXME magic number
    .wait = 25,
    .wait_count = 0
  };
  for(uint8_t i = 0; i < 32; i++) {
    lcd.characters[i] = LCD__;
  }
  return lcd;
}

// need to be able to handle the row changes...
LCDCommand lcd_next_command(LCD *lcd) {
  uint8_t data;
  uint8_t rs;

  // if there are no more indexes, reset and increment state
  if(lcd->characters_index < 32) {
    // assumes a valid index of 0 - 32
    if(lcd->characters_index == 0 && lcd->rows_index != 0) {
      rs = 0;
      data = 0x80;
      lcd->rows_index = 0;
    } else if(lcd->characters_index == 16 && lcd->rows_index != 1) {
      rs = 0;
      data = 0xC0;
      lcd->rows_index = 1;
    } else {
      rs = 1;
      data = lcd->characters[lcd->characters_index];
      lcd->characters_index++;
    }
  } else {
    // TODO this is a WIP for moving the cursor after writing the screen
    rs = 0;
    data = lcd->selected_position;
    lcd->characters_index = 0;
    lcd->mode = LCD_MODE_WAIT;
  }

  LCDCommand command = {
    .rs = rs,
    .data = data
  };

  return command;
}

void lcd_wait(LCD *lcd) {
  if(lcd->wait_count < lcd->wait) {
    lcd->wait_count++;
  } else {
    lcd->mode = LCD_MODE_WRITE;
    lcd->wait_count = 0;
  }
}

LCDCommand lcd_handle_interrupt(LCD *lcd) {
  LCDCommand command;

  if(lcd->mode == LCD_MODE_READ) {
    command = lcd_next_command(lcd);
  } else {
    command.rs = LCD_COMMAND_NULL_RS;
    command.data = LCD_COMMAND_NULL_DATA;
  }

  if(lcd->mode == LCD_MODE_WAIT) {
    lcd_wait(lcd);
  }

  return command;
}
