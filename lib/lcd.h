/**
 * @file lcd.h
 *
 * 16x02 style LCD state management.
 */

#ifndef _LCD_H_
#define _LCD_H_

#include "stdbool.h"
#include "stdint.h"
#include <stdio.h>

/**
 * LCD character codes.
 */
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

/**
 * Data and RS pin state for writing a command to the LCD.
 */
typedef struct LCDCommand {
  volatile uint8_t rs;
  volatile uint8_t data;
} LCDCommand;

#define LCD_COMMAND_NULL_RS 0x00
#define LCD_COMMAND_NULL_DATA 0x00

/**
 * LCD operational lifecycle states.
 */
typedef enum {
  LCD_MODE_WRITE,
  LCD_MODE_READ,
  LCD_MODE_WAIT
} LCD_MODE;

/**
 * Current value of all LCD characters, and metadata for writer.
 */
typedef volatile struct LCD {
  LCD_CHAR characters[32];
  int8_t characters_index;
  LCD_MODE mode;
  int8_t rows_index;
  uint8_t selected_position;
  uint16_t wait;
  uint16_t wait_count;
} LCD;

/**
 * Function signature for system specific command writing.
 */
typedef void (*LCDSendFunction)(bool rs, uint8_t data);

/**
 * TODO
 */
LCD lcd_new(void);

/**
 * TODO
 */
LCDCommand lcd_next_command(LCD *lcd);

/**
 * TODO
 */
void lcd_handle_interrupt(
  LCD *lcd,
  LCDSendFunction lcd_send
);

/**
 * TODO
 */
LCD_CHAR lcd_digit_to_char(uint8_t digit);

/**
 * TODO
 */
void lcd_wait(LCD *lcd);

#endif
