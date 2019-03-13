#include "headwater_ui.h"

uint8_t headwater_ui_modify_with_restrictions(
  uint8_t current,
  int16_t modifier,
  uint8_t min,
  uint8_t max
) {
  if(modifier < 0 && (current - min) < (modifier * -1)) {
    return min;
  } else if(0 < modifier && (max - current) < modifier) {
    return max;
  } else {
    return current + modifier;
  }
}

uint16_t headwater_ui_modify_with_restrictions_16(
  uint16_t current,
  int16_t modifier,
  uint16_t min,
  uint16_t max
) {
  if(modifier < 0 && (current - min) < (modifier * -1)) {
    return min;
  } else if(0 < modifier && (max - current) < modifier) {
    return max;
  } else {
    return current + modifier;
  }
}

uint16_t headwater_ui_modify_bpm(uint16_t value, int16_t modifier) {
  return headwater_ui_modify_with_restrictions_16(value, modifier, 10, 9999);
}

void main_bpm_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->bpm = headwater_ui_modify_bpm(
    state->bpm,
    (field->uncommitted_modifier * 10)
  );
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
}

void main_bpm_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint16_t value = headwater_ui_modify_bpm(
    state->bpm,
    (field->uncommitted_modifier * 10)
  );
  headwater_lcd_update_main_bpm(lcd, value);
}

void main_tbpm_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->bpm = headwater_ui_modify_bpm(
    state->bpm,
    field->uncommitted_modifier
  );
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
}

void main_tbpm_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint16_t value = headwater_ui_modify_bpm(
    state->bpm,
    field->uncommitted_modifier
  );
  headwater_lcd_update_main_bpm(lcd, value);
}

uint16_t headwater_ui_modify_2digit(uint8_t value, int16_t modifier) {
  return headwater_ui_modify_with_restrictions(value, modifier, 0, 99);
}

void main_multiplier_a_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->multiplier_a_channel.multiplier = headwater_ui_modify_2digit(
    state->multiplier_a_channel.multiplier,
    field->uncommitted_modifier
  );
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
}

void main_multiplier_a_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint8_t value = headwater_ui_modify_2digit(
    state->multiplier_a_channel.multiplier,
    field->uncommitted_modifier
  );
  headwater_lcd_update_main_multiplier_a(lcd, value);
}

void main_multiplier_b_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->multiplier_b_channel.multiplier = headwater_ui_modify_2digit(
    state->multiplier_b_channel.multiplier,
    field->uncommitted_modifier
  );
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
}

void main_multiplier_b_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint8_t value = headwater_ui_modify_2digit(
    state->multiplier_b_channel.multiplier,
    field->uncommitted_modifier
  );
  headwater_lcd_update_main_multiplier_b(lcd, value);
}

void main_preset_update_state(
  UIField *field,
  HeadwaterState *state
) {
  // TODO load / save state
  state->preset = headwater_ui_modify_2digit(
    state->preset,
    field->uncommitted_modifier
  );
}

void main_preset_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint8_t value = headwater_ui_modify_2digit(
    state->preset,
    field->uncommitted_modifier
  );
  headwater_lcd_update_main_preset(lcd, value);
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

  UIField main_preset_field = {
    .selected_position = 0x87,
    .update_state = &main_preset_update_state,
    .update_display = &main_preset_update_display
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

  // TODO mode field

  UIField main_fields[5] = {
    main_bpm_field,
    main_tbpm_field,
    main_preset_field,
    main_multiplier_a_field,
    main_multiplier_b_field
  };

  UIScreen main_screen = ui_screen_new(state, lcd, main_fields, 5);

  return main_screen;
}
