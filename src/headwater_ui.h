#ifndef _HEADWATER_UI_H_
#define _HEADWATER_UI_H_

#include "lcd.h"
#include "headwater_lcd.h"
#include "headwater_state.h"

#define UI_DISPLAY LCD
#define UI_STATE HeadwaterState

#include "ui.h"

uint8_t headwater_ui_modify_with_restrictions(
  uint8_t current,
  int16_t modifier,
  uint8_t min,
  uint8_t max
);
uint16_t headwater_ui_modify_with_restrictions_16(
  uint16_t current,
  int16_t modifier,
  uint16_t min,
  uint16_t max
);
UIScreen headwater_ui_main_screen(HeadwaterState *state, LCD *display);

#endif
