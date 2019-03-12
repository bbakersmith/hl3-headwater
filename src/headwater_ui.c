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

void main_multiplier_a_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->multiplier_a_channel.multiplier += (field->uncommitted_modifier);
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
}

void main_multiplier_a_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint16_t value =
    state->multiplier_a_channel.multiplier + (field->uncommitted_modifier);

  headwater_lcd_update_main_multiplier_a(lcd, value);
}

void main_multiplier_b_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->multiplier_b_channel.multiplier += (field->uncommitted_modifier);
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
}

void main_multiplier_b_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint16_t value =
    state->multiplier_b_channel.multiplier + (field->uncommitted_modifier);

  headwater_lcd_update_main_multiplier_b(lcd, value);
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

  UIField main_multiplier_a_field = {
    .selected_position = 0xC1,
    .update_state = &main_multiplier_a_update_state,
    .update_display = &main_multiplier_a_update_display
  };

  UIField main_multiplier_b_field = {
    .selected_position = 0xC4,
    .update_state = &main_multiplier_b_update_state,
    .update_display = &main_multiplier_b_update_display
  };

  UIField main_fields[4] = {
    main_bpm_field,
    main_tbpm_field,
    main_multiplier_a_field,
    main_multiplier_b_field
  };

  UIScreen main_screen = ui_screen_new(state, lcd, main_fields, 4);

  return main_screen;
}
