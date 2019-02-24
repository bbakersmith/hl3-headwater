#ifndef _UI_H_
#define _UI_H_

#include "stdint.h"

#ifdef UI_STATE
#define UIState UI_STATE
#else
typedef struct UIState {} UIState;
#endif

typedef enum {
  UI_SCREEN_DIRECTION_LEFT,
  UI_SCREEN_DIRECTION_RIGHT
} UI_SCREEN_DIRECTION;

typedef struct UIField {
  uint8_t selected_position;
  void (*update_state)(UIState *state, int8_t modifier);
  void (*update_lcd)(uint8_t *characters[32], UIState *state);
} UIField;

typedef volatile struct UIScreen {
  UIField fields[8];
  uint8_t last_index;
  uint8_t select_index;
  uint8_t update_index;
} UIScreen;

UIScreen ui_screen_new(UIField fields[8], uint8_t fields_count);
uint8_t ui_selected_position(UIScreen *screen);
void ui_move_selected(
  UIScreen *screen,
  UI_SCREEN_DIRECTION direction
);

#endif
