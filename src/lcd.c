#include <stdio.h>
#include "stdint.h"

#include "lcd.h"

LCDField lcd_field_new(uint8_t id, uint8_t position) {
  LCDField field = {
    .id = id,
    .position = position
  };
  return field;
}

uint8_t lcd_is_field_null(LCDField field) {
  if(field.id == LCD_FIELD_NULL_ID) {
    return 1;
  } else {
    return 0;
  }
}

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

LCDScreen lcd_screen_new(LCDField fields[8], uint8_t fields_length) {
  LCDScreen screen = {
    .change_flags = 0,
    .fields_index = 0,
    .fields_length = fields_length
  };
  for(uint8_t i = 0; i < fields_length; i++) {
    screen.fields[i] = fields[i];
    screen.change_flags |= (1 << i);
  }
  return screen;
}

LCD lcd_new(void) {
  LCD lcd = {
    .characters_index = 0,
    .wait = 25, // TODO best default?
    .wait_count = 0,
    .mode = LCD_MODE_WRITE,
    .rows_index = -1
  };
  for(uint8_t i = 0; i < 32; i++) {
    lcd.characters[i] = LCD__;
  }
  return lcd;
}

LCDField lcd_next_changed_field(LCDScreen *screen) {
  LCDField field;
  while(1) {
    if(screen->fields_index < screen->fields_length) {
      if(screen->change_flags & (1 << screen->fields_index)) {
        field = screen->fields[screen->fields_index];
        screen->fields_index++;
        break;
      }
      screen->fields_index++;
    } else {
      screen->fields_index = 0;
      field = LCDFieldNull;
      break;
    }
  }
  return field;
}

// need to be able to handle the row changes...
LCDCommand lcd_next_command(LCD *lcd) {
  uint8_t data;
  uint8_t rs;

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

  // if there are no more indexes, reset and increment state
  if(32 <= lcd->characters_index) {
    lcd->characters_index = 0;
    lcd->mode = LCD_MODE_WAIT;
  }

  LCDCommand command = {
    .rs = rs,
    .data = data
  };

  return command;
}

uint8_t lcd_is_command_null(LCDCommand command) {
  if(command.rs == LCD_COMMAND_NULL_RS) {
    if(command.data == LCD_COMMAND_NULL_DATA) {
      return 1;
    } else {
      return 0;
    }
  }
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
    command = LCDCommandNull;
  }

  if(lcd->mode == LCD_MODE_WAIT) {
    lcd_wait(lcd);
  }

  return command;
}

// write byte to shift register

// write byte to lcd

// create main screen

// create message screen

// display timed message

// display main screen value
