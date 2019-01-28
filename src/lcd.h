#ifndef _LCD_H_
#define _LCD_H_

#ifdef LCD_STATE
#define LCDState LCD_STATE
#endif

#include "stdint.h"

// typedef enum {
//   LCD_ROW_1_COL_1 = 0x80,
//   LCD_ROW_1_COL_2,
//   LCD_ROW_1_COL_3,
//   LCD_ROW_1_COL_4,
//   LCD_ROW_1_COL_5,
//   LCD_ROW_1_COL_6,
//   LCD_ROW_1_COL_7,
//   LCD_ROW_1_COL_8,
//   LCD_ROW_1_COL_9,
//   LCD_ROW_1_COL_10,
//   LCD_ROW_1_COL_11,
//   LCD_ROW_1_COL_12,
//   LCD_ROW_1_COL_13,
//   LCD_ROW_1_COL_14,
//   LCD_ROW_1_COL_15,
//   LCD_ROW_1_COL_16,
//   LCD_ROW_2_COL_1 = 0xC0,
//   LCD_ROW_2_COL_2,
//   LCD_ROW_2_COL_3,
//   LCD_ROW_2_COL_4,
//   LCD_ROW_2_COL_5,
//   LCD_ROW_2_COL_6,
//   LCD_ROW_2_COL_7,
//   LCD_ROW_2_COL_8,
//   LCD_ROW_2_COL_9,
//   LCD_ROW_2_COL_10,
//   LCD_ROW_2_COL_11,
//   LCD_ROW_2_COL_12,
//   LCD_ROW_2_COL_13,
//   LCD_ROW_2_COL_14,
//   LCD_ROW_2_COL_15,
//   LCD_ROW_2_COL_16
// } LCD_POSITION;

typedef enum {
  LCD__ = 0x20,
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

typedef struct LCDField {
  uint8_t id;
  uint8_t position;
} LCDField;

#define LCD_FIELD_NULL_ID 0
static LCDField LCDFieldNull = {.id = LCD_FIELD_NULL_ID};

typedef struct LCDCommand {
  volatile uint8_t rs;
  volatile uint8_t data;
} LCDCommand;

#define LCD_COMMAND_NULL_RS 0x00
#define LCD_COMMAND_NULL_DATA 0x00

static LCDCommand LCDCommandNull = {
  .rs = LCD_COMMAND_NULL_RS,
  .data = LCD_COMMAND_NULL_DATA
};

typedef struct LCDScreen {
  uint8_t change_flags;
  LCDField fields[8];
  uint8_t fields_index;
  uint8_t fields_length;
} LCDScreen;

typedef enum {
  LCD_MODE_WRITE,
  LCD_MODE_READ,
  LCD_MODE_WAIT
} LCD_MODE;

typedef volatile struct LCD {
  LCD_CHAR characters[32];
  int8_t characters_index;
  uint16_t wait;
  uint16_t wait_count;
  LCD_MODE mode;
  int8_t rows_index;
} LCD;

LCDField lcd_field_new(uint8_t id, uint8_t position);
LCDScreen lcd_screen_new(LCDField fields[8], uint8_t fields_length);
LCD lcd_new(void);
LCDField lcd_next_changed_field(LCDScreen *screen);
LCDCommand lcd_next_command(LCD *lcd);
LCDCommand lcd_handle_interrupt(LCD *lcd);
uint8_t lcd_is_field_null(LCDField field);
uint8_t lcd_is_command_null(LCDCommand command);
void lcd_wait(LCD *lcd);

#endif
