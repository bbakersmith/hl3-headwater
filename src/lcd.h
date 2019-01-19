#ifndef _LCD_H_
#define _LCD_H_

#include "stdint.h"

typedef enum {
  LCD_ROW_1_COL_1 = 0x80,
  LCD_ROW_1_COL_2,
  LCD_ROW_1_COL_3,
  LCD_ROW_1_COL_4,
  LCD_ROW_1_COL_5,
  LCD_ROW_1_COL_6,
  LCD_ROW_1_COL_7,
  LCD_ROW_1_COL_8,
  LCD_ROW_1_COL_9,
  LCD_ROW_1_COL_10,
  LCD_ROW_1_COL_11,
  LCD_ROW_1_COL_12,
  LCD_ROW_1_COL_13,
  LCD_ROW_1_COL_14,
  LCD_ROW_1_COL_15,
  LCD_ROW_1_COL_16,
  LCD_ROW_2_COL_1 = 0xC0,
  LCD_ROW_2_COL_2,
  LCD_ROW_2_COL_3,
  LCD_ROW_2_COL_4,
  LCD_ROW_2_COL_5,
  LCD_ROW_2_COL_6,
  LCD_ROW_2_COL_7,
  LCD_ROW_2_COL_8,
  LCD_ROW_2_COL_9,
  LCD_ROW_2_COL_10,
  LCD_ROW_2_COL_11,
  LCD_ROW_2_COL_12,
  LCD_ROW_2_COL_13,
  LCD_ROW_2_COL_14,
  LCD_ROW_2_COL_15,
  LCD_ROW_2_COL_16
} LCD_POSITION;

typedef enum {
  LCD__A = 0x41,
  LCD__B,
  LCD__C,
  LCD__D,
  LCD__E,
  LCD__F,
  LCD__G,
  LCD__H,
  LCD__I,
  LCD__J,
  LCD__K,
  LCD__L,
  LCD__M,
  LCD__N,
  LCD__O,
  LCD__P,
  LCD__Q,
  LCD__R,
  LCD__S,
  LCD__T,
  LCD__U,
  LCD__V,
  LCD__W,
  LCD__X,
  LCD__Y,
  LCD__Z
} LCD_CHAR;

typedef struct LCDScreenField {
  int8_t index;
  uint8_t length;
  LCD_POSITION position; // 0x00 - 0x1F
  LCD_CHAR value[16];
} LCDScreenField;

// TODO a linked list of fields is a screen!

typedef struct LCDScreen {
  LCDScreenField fields[8];
  uint8_t index;
  uint8_t length;
} LCDScreen;

typedef volatile struct LCDState {
  uint8_t index;
  LCDScreen screens[8];
  void (*write_cmd_fn)(uint8_t value);
  void (*write_data_fn)(uint8_t value);
} LCDState;

LCDScreenField lcd_screen_field_new(
  LCD_CHAR value[16],
  uint8_t length,
  LCD_POSITION position
);

LCDScreen lcd_screen_new(LCDScreenField fields[8], uint8_t length);
LCDScreen lcd_screen_message_top_new(LCD_CHAR message[16], uint8_t length);
void lcd_handle_interrupt(LCDState *state);
uint8_t lcd_position_address(LCD_POSITION position);

// TODO how to construct message queue?
// should really know about the values that are on screen
// and dedupe rather than queue all

// should continually refresh the state of the current screen
// cycling through each value, ideally only updating if changed
// but also whatever...
// start by simply redrawing all values repeatedly

#endif
