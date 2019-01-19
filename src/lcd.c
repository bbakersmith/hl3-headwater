#include <stdio.h>
#include "stdint.h"

#include "lcd.h"

LCD_POSITION lcd_center_position(uint8_t length) {
  LCD_POSITION position;
  uint8_t whitespace = 16 - length;

  if(whitespace == 0) {
    position = 0;
  } else {
    position = whitespace / 2;
  }

  return position;
}

LCDScreenField lcd_screen_field_new(
  LCD_CHAR value[16],
  uint8_t length,
  LCD_POSITION position
) {
  LCDScreenField field;
  field.index = -1;
  field.position = position;

  for(uint8_t i = 0; i < length; i++) {
    field.value[i] = value[i];
  }

  field.length = length;
  return field;
}

LCDScreen lcd_screen_new(LCDScreenField fields[8], uint8_t length) {
  LCDScreen screen;
  screen.index = 0;

  for(uint8_t i = 0; i < length; i++) {
    screen.fields[i] = fields[i];
  }

  screen.length = length;
  return screen;
}

LCDScreen lcd_screen_message_top_new(LCD_CHAR message[16], uint8_t length) {
  LCDScreenField field = lcd_screen_field_new(
    message,
    length,
    LCD_ROW_1_COL_1
  );
  LCDScreenField fields[8] = {field};
  LCDScreen screen = lcd_screen_new(fields, 1);
  return screen;
}

void lcd_handle_interrupt(LCDState *state) {
  LCDScreen volatile *screen = &state->screens[state->index];

  // reset if all fields complete
  if(screen->length <= screen->index) {
    screen->index = 0;
  }

  LCDScreenField *initial_field = &screen->fields[screen->index];

  // next field if current field complete
  if(initial_field->length <= initial_field->index) {
    initial_field->index = -1;
    screen->index++;
  }

  LCDScreenField *field = &screen->fields[screen->index];

  // if new field move cursor, otherwise write data
  if(field->index == -1) {
    state->write_cmd_fn(field->position);
  } else if(field->index < field->length) {
    state->write_data_fn(field->value[field->index]);
  }

  field->index++;
}

// write byte to shift register

// write byte to lcd

// create main screen

// create message screen

// display timed message

// display main screen value
