#include "headwater_ui.h"

void main_bpm_update_state(
  UIField *field,
  HeadwaterState *state
) {
  state->bpm += (field->uncommitted_modifier * 10);
  headwater_api.state.change_flags |=
    (1 << HEADWATER_STATE_CHANGE_BPM);
}

void main_bpm_update_display(
  UIField *field,
  HeadwaterState *state,
  LCD *lcd
) {
  // TODO update display when selected

  // TODO need a way to indicate change in modified (blinking) state

  // if modifier != 0
  // - enable blinking, first time only
  // else
  // - disable blinking, first time onlly

  uint16_t value = state->bpm + (field->uncommitted_modifier * 10);
  headwater_lcd_update_main_bpm(lcd, value);
}

UIScreen headwater_ui_main_screen(void) {

  UIField main_bpm_field = {
    .selected_position = 0x82,
    .update_state = &main_bpm_update_state,
    .update_display = &main_bpm_update_display
  };

  UIField main_tbpm_field = {
    .selected_position = 0x84
  };

  UIField main_field_3 = {
    .selected_position = 0x86
  };

  UIField main_fields[3] = {
    main_bpm_field,
    main_tbpm_field,
    main_field_3
  };

  main_screen = ui_screen_new(main_fields, 3);

}
