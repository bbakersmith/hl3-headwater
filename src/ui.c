#include "ui.h"

UIScreen ui_screen_new(UIField fields[8], uint8_t fields_count) {
  UIScreen screen = {
    .last_index = fields_count - 1,
    .select_index = 0,
    .update_index = 0
  };
  for(uint8_t i = 0; i < fields_count; i++) {
    screen.fields[i] = fields[i];
  }
  return screen;
}

uint8_t ui_selected_position(UIScreen *screen) {
  return screen->fields[screen->select_index].selected_position;
}

void ui_move_selected(
  UIScreen *screen,
  UI_SCREEN_DIRECTION direction
) {
  if(direction == UI_SCREEN_DIRECTION_DEC) {
    if(0 < screen->select_index) {
      screen->select_index -= 1;
    } else {
      screen->select_index = screen->last_index;
    }
  } else if(direction == UI_SCREEN_DIRECTION_INC) {
    if(screen->select_index < screen->last_index) {
      screen->select_index += 1;
    } else {
      screen->select_index = 0;
    }
  }
}
