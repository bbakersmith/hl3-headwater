#ifndef _HEADWATER_UI_H_
#define _HEADWATER_UI_H_

#include "bytes.h"
#include "debounce.h"
#include "lcd.h"
#include "headwater_state.h"

#define UI_DISPLAY LCD
#define UI_STATE HeadwaterState

#include "ui.h"

#define HEADWATER_UI_PRESET_SIZE_MAX 10
#define HEADWATER_UI_PRESET_SIZE 6

typedef volatile struct HeadwaterUIInputs {
  DebounceButton stop_button;
  DebounceButton play_button;
  DebounceEncoder rotary_encoder;
  DebounceButton rotary_encoder_button;
  DebounceButton left_button;
  DebounceButton right_button;
  DebounceButton save_button;
} HeadwaterUIInputs;

typedef uint8_t (*HeadwaterUIEEPROMRead)(uint16_t address);
typedef void (*HeadwaterUIEEPROMWrite)(uint16_t address, uint8_t data);

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
void headwater_ui_generic_bpm_update_display(
  LCD *lcd,
  uint8_t position,
  uint16_t value
);
UIScreen headwater_ui_main_screen(
  HeadwaterState *state,
  LCD *display
);
void headwater_ui_load_preset(
  UIScreen *screen,
  HeadwaterState *state,
  HeadwaterUIEEPROMRead eeprom_read
);
void headwater_ui_save_preset(
  UIScreen *screen,
  HeadwaterState *state,
  HeadwaterUIEEPROMWrite eeprom_write
);
void headwater_ui_update_selected_state(
  UIScreen *screen,
  HeadwaterState *state,
  HeadwaterUIEEPROMRead eeprom_read
);
HeadwaterUIInputs headwater_ui_inputs_new(void);
void headwater_ui_handle_inputs(
  HeadwaterUIInputs *inputs,
  uint8_t input_flags,
  UIScreen *screen,
  HeadwaterState *state,
  HeadwaterUIEEPROMRead eeprom_read,
  HeadwaterUIEEPROMWrite eeprom_write
);

#endif
