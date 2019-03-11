#include "headwater_ui.h"

void main_bpm_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->bpm += (field->uncommitted_modifier * 10);
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
}

void main_bpm_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  // TODO need a way to indicate change in modified (blinking) state
  //
  // if modifier != 0
  // - enable blinking, first time only
  // else
  // - disable blinking, first time onlly

  uint16_t value = state->bpm + (field->uncommitted_modifier * 10);
  headwater_lcd_update_main_bpm(lcd, value);
}

void main_tbpm_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->bpm += (field->uncommitted_modifier);
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
}

void main_tbpm_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint16_t value = state->bpm + (field->uncommitted_modifier);
  headwater_lcd_update_main_bpm(lcd, value);

  /* if(field->uncommitted_modifer != 0) { */
  /*   headwater_lcd */
  /* } */
}

UIScreen headwater_ui_main_screen(
  HeadwaterState *state,
  LCD *lcd
) {

  UIField main_bpm_field = {
    .selected_position = 0x82,
    .update_state = &main_bpm_update_state,
    .update_display = &main_bpm_update_display
  };

  UIField main_tbpm_field = {
    .selected_position = 0x84,
    .update_state = &main_tbpm_update_state,
    .update_display = &main_tbpm_update_display
  };

  UIField main_fields[2] = {
    main_bpm_field,
    main_tbpm_field
  };

  UIScreen main_screen = ui_screen_new(state, lcd, main_fields, 2);

  return main_screen;
}
