#ifndef _UI_H_
#define _UI_H_

#include "stdint.h"

#ifdef UI_DISPLAY
#define UIDisplay UI_DISPLAY
#else
typedef struct UIDisplay {} UIDisplay;
#endif

#ifdef UI_STATE
#define UIState UI_STATE
#else
typedef struct UIState {} UIState;
#endif

typedef enum {
  UI_FIELD_MODIFIER_DEC,
  UI_FIELD_MODIFIER_INC
} UI_FIELD_MODIFIER;

typedef enum {
  UI_SCREEN_DIRECTION_DEC,
  UI_SCREEN_DIRECTION_INC
} UI_SCREEN_DIRECTION;

typedef struct UIField {
  uint8_t selected_position;
  volatile int16_t uncommitted_modifier;
  // TODO react to change, future only when re sw is activated
  void (*update_state)(struct UIField *field, UIState *state);
  // TODO highlight the selected field, future display not-yet-committed value
  void (*update_display)(
    struct UIField *field,
    UIDisplay *display,
    UIState *state
  );
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
