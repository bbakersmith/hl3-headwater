#include "headwater_ui.h"

// FIXME needs to stop modifier from increasing
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

// FIXME needs to stop modifier from increasing
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
  uint8_t value = headwater_ui_modify_with_restrictions(
      state->mode,
      field->uncommitted_modifier,
      HEADWATER_STATE_MODE_INTERNAL,
      HEADWATER_STATE_MODE_MIDI
  );
  state->mode = value;
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

  lcd->characters[position - 1] = char1;
  lcd->characters[position] = char2;
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

void headwater_ui_load_preset(
  UIScreen *screen,
  HeadwaterState *state,
  HeadwaterUIEEPROMRead eeprom_read
) {
  uint8_t preset =
    state->preset + screen->fields[screen->select_index].uncommitted_modifier;

  // read bpm, a, b, mode
  uint8_t data[HEADWATER_UI_PRESET_SIZE];

  // if first byte (preset number) is not equal to the current
  // preset number, assume corrupted and do nothing
  data[0] = eeprom_read(preset * HEADWATER_UI_PRESET_SIZE_MAX);

  if(data[0] != preset) return;

  ui_update_selected_state(screen);

  // read preset data
  for(uint16_t i = 1; i < HEADWATER_UI_PRESET_SIZE; i++) {
    uint16_t address = (preset * HEADWATER_UI_PRESET_SIZE_MAX) + i;
    data[i] = eeprom_read(address);
  }

  state->bpm = bytes_high_low_to_16bit(data[1], data[2]);
  state->multiplier_a_channel.multiplier = data[3];
  state->multiplier_b_channel.multiplier = data[4];
  state->mode = data[5];

  state->change_flags |=
    (1 << HEADWATER_STATE_CHANGE_MODE)
    | (1 << HEADWATER_STATE_CHANGE_BPM)
    | (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A)
    | (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B);
}

void headwater_ui_save_preset(
  UIScreen *screen,
  HeadwaterState *state,
  HeadwaterUIEEPROMWrite eeprom_write
) {
  if(screen->select_index == 5) {
    ui_update_selected_state(screen);
  }

  uint8_t preset = state->preset;
  uint16_t bpm = state->bpm;

  // write bpm, a, b, mode
  uint8_t data[HEADWATER_UI_PRESET_SIZE] = {
    preset,
    bytes_16bit_to_high(bpm),
    bytes_16bit_to_low(bpm),
    state->multiplier_a_channel.multiplier,
    state->multiplier_b_channel.multiplier,
    state->mode
  };

  // save preset data
  for(uint8_t i = 0; i < HEADWATER_UI_PRESET_SIZE; i++) {
    uint16_t address = (preset * HEADWATER_UI_PRESET_SIZE_MAX) + i;
    eeprom_write(address, data[i]);
  }
}

void headwater_ui_update_selected_state(
  UIScreen *screen,
  HeadwaterState *state,
  HeadwaterUIEEPROMRead eeprom_read
) {
  // FIXME preset field magic index
  if(screen->select_index == 5) {
    headwater_ui_load_preset(screen, state, eeprom_read);
    screen->change_flags = 0xFF;
  } else {
    ui_update_selected_state(screen);
  }
}

HeadwaterUIInputs headwater_ui_inputs_new(void) {
  uint8_t debounce_threshold = 5;

  HeadwaterUIInputs inputs = {
    .stop_button = debounce_button_new(
      DEBOUNCE_BUTTON_STATE_HIGH,
      debounce_threshold
    ),

    .play_button = debounce_button_new(
      DEBOUNCE_BUTTON_STATE_HIGH,
      debounce_threshold
    ),

    .rotary_encoder = debounce_encoder_new(
      DEBOUNCE_BUTTON_STATE_HIGH,
      debounce_threshold
    ),

    .rotary_encoder_button = debounce_button_new(
      DEBOUNCE_BUTTON_STATE_HIGH,
      debounce_threshold
    ),

    .left_button = debounce_button_new(
      DEBOUNCE_BUTTON_STATE_HIGH,
      debounce_threshold
    ),

    .right_button = debounce_button_new(
      DEBOUNCE_BUTTON_STATE_HIGH,
      debounce_threshold
    ),

    .save_button = debounce_button_new(
      DEBOUNCE_BUTTON_STATE_HIGH,
      debounce_threshold
    )
  };

  return inputs;
}

void headwater_ui_update_inputs(
  HeadwaterUIInputs *inputs,
  uint8_t input_flags
) {
  debounce_button_update(
    &inputs->stop_button,
    bytes_check_bit(input_flags, 0)
  );

  debounce_button_update(
    &inputs->play_button,
    bytes_check_bit(input_flags, 1)
  );

  debounce_encoder_update(
    &inputs->rotary_encoder,
    bytes_check_bit(input_flags, 3),
    bytes_check_bit(input_flags, 2)
  );

  debounce_button_update(
    &inputs->rotary_encoder_button,
    bytes_check_bit(input_flags, 4)
  );

  debounce_button_update(
    &inputs->left_button,
    bytes_check_bit(input_flags, 5)
  );

  debounce_button_update(
    &inputs->right_button,
    bytes_check_bit(input_flags, 6)
  );

  debounce_button_update(
    &inputs->save_button,
    bytes_check_bit(input_flags, 7)
  );
}

void headwater_ui_handle_inputs(
  HeadwaterUIInputs *inputs,
  uint8_t input_flags,
  UIScreen *screen,
  HeadwaterState *state,
  HeadwaterUIEEPROMRead eeprom_read,
  HeadwaterUIEEPROMWrite eeprom_write
) {
  headwater_ui_update_inputs(inputs, input_flags);

  if(inputs->stop_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
    state->change_flags |= (1 << HEADWATER_STATE_CHANGE_STOP);
  }

  if(inputs->play_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
    state->change_flags |= (1 << HEADWATER_STATE_CHANGE_PLAY);
  }

  if(inputs->rotary_encoder.output == DEBOUNCE_ENCODER_CHANGE_LEFT) {
    ui_update_selected_modifier(screen, -1);
    if(inputs->rotary_encoder_button.state == DEBOUNCE_BUTTON_STATE_LOW) {
      headwater_ui_update_selected_state(
        screen,
        state,
        eeprom_read
      );
    }
  }

  if(inputs->rotary_encoder.output == DEBOUNCE_ENCODER_CHANGE_RIGHT) {
    ui_update_selected_modifier(screen, 1);
    if(inputs->rotary_encoder_button.state == DEBOUNCE_BUTTON_STATE_LOW) {
      headwater_ui_update_selected_state(
        screen,
        state,
        eeprom_read
      );
    }
  }

  if(inputs->rotary_encoder_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
    headwater_ui_update_selected_state(
      screen,
      state,
      eeprom_read
    );
  }

  if(inputs->left_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
    ui_move_selected(screen, UI_SCREEN_DIRECTION_DEC);
    screen->display->selected_position = ui_selected_position(screen);
  }

  if(inputs->right_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
    ui_move_selected(screen, UI_SCREEN_DIRECTION_INC);
    screen->display->selected_position = ui_selected_position(screen);
  }

  if(inputs->save_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
    headwater_ui_save_preset(
      screen,
      state,
      eeprom_write
    );
  }
}
