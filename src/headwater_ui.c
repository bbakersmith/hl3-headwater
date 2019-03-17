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

void headwater_ui_generic_bpm_update_display(
  LCD *lcd,
  uint8_t position,
  uint16_t value
) {
  uint8_t digit;

  digit = value % 10;
  lcd->characters[position] = lcd_digit_to_char(digit);
  value /= 10;

  lcd->characters[position - 1] = LCD__DOT;

  digit = value % 10;
  lcd->characters[position - 2] = lcd_digit_to_char(digit);
  value /= 10;

  if(0 < value) {
    digit = value % 10;
    lcd->characters[position - 3] = lcd_digit_to_char(digit);
    value /= 10;
  } else {
    lcd->characters[position - 3] = LCD__;
  }

  if(0 < value) {
    digit = value % 10;
    lcd->characters[position - 4] = lcd_digit_to_char(digit);
  } else {
    lcd->characters[position - 4] = LCD__;
  }
}

void headwater_ui_bpm_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->bpm = headwater_ui_modify_bpm(
    state->bpm,
    (field->uncommitted_modifier * 10)
  );
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
}

void headwater_ui_bpm_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint16_t value = headwater_ui_modify_bpm(
    state->bpm,
    (field->uncommitted_modifier * 10)
  );
  headwater_ui_generic_bpm_update_display(
    lcd,
    field->selected_position + 2,
    value
  );
}

void headwater_ui_tbpm_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->bpm = headwater_ui_modify_bpm(
    state->bpm,
    field->uncommitted_modifier
  );
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
}

void headwater_ui_tbpm_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint16_t value = headwater_ui_modify_bpm(
    state->bpm,
    field->uncommitted_modifier
  );
  headwater_ui_generic_bpm_update_display(
    lcd,
    field->selected_position,
    value
  );
}

uint16_t headwater_ui_modify_2digit(uint8_t value, int16_t modifier) {
  return headwater_ui_modify_with_restrictions(value, modifier, 0, 99);
}

void headwater_ui_update_display_2digit(
  LCD *lcd,
  uint8_t position,
  uint8_t value
) {
  uint8_t digit;

  digit = value % 10;
  lcd->characters[position] = lcd_digit_to_char(digit);
  value /= 10;

  if(0 < value) {
    digit = value % 10;
    lcd->characters[position - 1] = lcd_digit_to_char(digit);
    value /= 10;
  } else {
    lcd->characters[position - 1] = LCD__;
  }
}

void headwater_ui_multiplier_a_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->multiplier_a_channel.multiplier = headwater_ui_modify_2digit(
    state->multiplier_a_channel.multiplier,
    field->uncommitted_modifier
  );
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
}

void headwater_ui_multiplier_a_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint8_t value = headwater_ui_modify_2digit(
    state->multiplier_a_channel.multiplier,
    field->uncommitted_modifier
  );
  headwater_ui_update_display_2digit(
    lcd,
    field->selected_position,
    value
  );
}

void headwater_ui_multiplier_b_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->multiplier_b_channel.multiplier = headwater_ui_modify_2digit(
    state->multiplier_b_channel.multiplier,
    field->uncommitted_modifier
  );
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B);
}

void headwater_ui_multiplier_b_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint8_t value = headwater_ui_modify_2digit(
    state->multiplier_b_channel.multiplier,
    field->uncommitted_modifier
  );
  headwater_ui_update_display_2digit(
    lcd,
    field->selected_position,
    value
  );
}

void headwater_ui_mode_update_state(
  UIField *field,
  HeadwaterState *state
) {
  // TODO actually update mode state
  state->change_flags |= (1 << HEADWATER_STATE_CHANGE_MODE);
}

void headwater_ui_mode_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint8_t value = headwater_ui_modify_with_restrictions(
      state->mode,
      field->uncommitted_modifier,
      HEADWATER_STATE_MODE_INTERNAL,
      HEADWATER_STATE_MODE_MIDI
  );

  uint8_t position = field->selected_position;
  uint8_t char1;
  uint8_t char2;

  switch(value) {
    case HEADWATER_STATE_MODE_INTERNAL:
      char1 = LCD__I;
      char2 = LCD__N;
      break;
    case HEADWATER_STATE_MODE_TAP:
      char1 = LCD__T;
      char2 = LCD__P;
      break;
    case HEADWATER_STATE_MODE_EXTERNAL:
      char1 = LCD__E;
      char2 = LCD__X;
      break;
    case HEADWATER_STATE_MODE_MIDI:
      char1 = LCD__M;
      char2 = LCD__I;
      break;
    default:
      char1 = LCD__QUES;
      char2 = LCD__QUES;
  }

  lcd->characters[position] = char1;
  lcd->characters[position + 1] = char2;
}

void headwater_ui_preset_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->preset = headwater_ui_modify_2digit(
    state->preset,
    field->uncommitted_modifier
  );
}

void headwater_ui_preset_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  uint8_t value = headwater_ui_modify_2digit(
    state->preset,
    field->uncommitted_modifier
  );
  headwater_ui_update_display_2digit(
    lcd,
    field->selected_position,
    value
  );
}

UIScreen headwater_ui_main_screen(
  HeadwaterState *state,
  LCD *lcd
) {

  UIField main_bpm_field = {
    .selected_position = 2,
    .update_state = &headwater_ui_bpm_update_state,
    .update_display = &headwater_ui_bpm_update_display
  };

  UIField main_tbpm_field = {
    .selected_position = 4,
    .update_state = &headwater_ui_tbpm_update_state,
    .update_display = &headwater_ui_tbpm_update_display
  };

  UIField main_mode_field = {
    .selected_position = 7,
    .update_state = &headwater_ui_mode_update_state,
    .update_display = &headwater_ui_mode_update_display
  };

  UIField main_multiplier_a_field = {
    .selected_position = 17,
    .update_state = &headwater_ui_multiplier_a_update_state,
    .update_display = &headwater_ui_multiplier_a_update_display
  };

  UIField main_multiplier_b_field = {
    .selected_position = 20,
    .update_state = &headwater_ui_multiplier_b_update_state,
    .update_display = &headwater_ui_multiplier_b_update_display
  };

  UIField main_preset_field = {
    .selected_position = 23,
    .update_state = &headwater_ui_preset_update_state,
    .update_display = &headwater_ui_preset_update_display
  };

  UIField main_fields[6] = {
    main_bpm_field,
    main_tbpm_field,
    main_mode_field,
    main_multiplier_a_field,
    main_multiplier_b_field,
    main_preset_field
  };

  UIScreen main_screen = ui_screen_new(state, lcd, main_fields, 6);

  return main_screen;
}
