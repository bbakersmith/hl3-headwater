#ifndef _LCD_H_
#define _LCD_H_

#ifdef LCD_STATE
#define LCDState LCD_STATE
#endif

#include "stdint.h"
#include <stdio.h>

typedef enum {
  LCD__ = 0x20,
  LCD__BANG,
  LCD__DQUOTE,
  LCD__NUM,
  LCD__USD,
  LCD__PCT,
  LCD__AND,
  LCD__QUOTE,
  LCD__LPAREN,
  LCD__RPAREN,
  LCD__STAR,
  LCD__PLUS,
  LCD__COMMA,
  LCD__DASH,
  LCD__DOT,
  LCD__FSLASH,
  LCD__0,
  LCD__1,
  LCD__2,
  LCD__3,
  LCD__4,
  LCD__5,
  LCD__6,
  LCD__7,
  LCD__8,
  LCD__9,
  LCD__COLON,
  LCD__SEMI,
  LCD__LT,
  LCD__EQ,
  LCD__GT,
  LCD__QUES,
  LCD__AT,
  LCD__A,
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

LCD lcd_new(void);
LCDCommand lcd_next_command(LCD *lcd);
LCDCommand lcd_handle_interrupt(LCD *lcd);
LCD_CHAR lcd_digit_to_char(uint8_t digit);
uint8_t lcd_is_command_null(LCDCommand command);
void lcd_wait(LCD *lcd);
void lcd_load_inverted_charset(void (*send_fn)(uint8_t rs, uint8_t data));

#endif
