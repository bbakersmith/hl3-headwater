#include "ui.h"

UIScreen ui_screen_new(
  UIState *state,
  UIDisplay *display,
  UIField fields[8],
  uint8_t fields_count
) {
  UIScreen screen = {
    .change_flags = 0xFF,
    .last_index = fields_count - 1,
    .select_index = 0,
    .update_index = 0
  };
  for(uint8_t i = 0; i < fields_count; i++) {
    screen.fields[i] = fields[i];
  }
  screen.state = state;
  screen.display = display;
  return screen;
}

uint8_t ui_selected_position(UIScreen *screen) {
  return screen->fields[screen->select_index].selected_position;
}

int16_t ui_selected_modifier(UIScreen *screen) {
  return screen->fields[screen->select_index].uncommitted_modifier;
}

void ui_move_selected(
  UIScreen *screen,
  UI_SCREEN_DIRECTION direction
) {
  screen->fields[screen->select_index].uncommitted_modifier = 0;
  screen->change_flags |= (1 << screen->select_index);

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

  screen->change_flags |= (1 << screen->select_index);
}

void ui_update_selected_modifier(
  UIScreen *screen,
  int8_t modifier
) {
  screen->fields[screen->select_index].uncommitted_modifier += modifier;
  screen->change_flags |= (1 << screen->select_index);
}

void ui_update_selected_display(UIScreen *screen) {
  UIField selected_field = screen->fields[screen->select_index];
  selected_field.update_display(
    &selected_field,
    screen->state,
    screen->display
  );
}

void ui_update_selected_state(UIScreen *screen) {
  UIField selected_field = screen->fields[screen->select_index];
  selected_field.update_state(&selected_field, screen->state);
  screen->fields[screen->select_index].uncommitted_modifier = 0;
}

void ui_update_changed_display(UIScreen *screen) {
  // FIXME 2 is field count
  for(uint8_t i = 0; i < 2; i++) {
    if(screen->change_flags & (1 << i)) {
      screen->change_flags &= ~(1 << i);

      UIField field = screen->fields[i];
      field.update_display(
        &field,
        screen->state,
        screen->display
      );
    }
  }
}
