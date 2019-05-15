/**
 * @file ui.h
 */

#ifndef _UI_H_
#define _UI_H_

#include "stdint.h"

#ifndef UIDisplay
#include "dummy_struct.h"
#define UIDisplay DummyStruct
#endif

#ifndef UIState
#include "dummy_struct.h"
#define UIState DummyStruct
#endif

typedef enum {
  UI_FIELD_MODIFIER_DEC,
  UI_FIELD_MODIFIER_INC
} UIFieldModifier;

typedef enum {
  UI_SCREEN_DIRECTION_DEC,
  UI_SCREEN_DIRECTION_INC
} UIScreenDirection;

typedef enum {
  UI_DISPLAY_CHANGED_FALSE,
  UI_DISPLAY_CHANGED_TRUE
} UIDisplayChanged;

typedef struct UIField {
  uint8_t selected_position;
  volatile int16_t uncommitted_modifier;
  void (*update_state)(struct UIField *field, UIState *state);
  void (*update_display)(
    struct UIField *field,
    UIState *state,
    UIDisplay *display
  );
} UIField;

typedef volatile struct UIScreen {
  uint8_t change_flags;
  UIDisplay *display;
  UIField fields[8];
  uint8_t fields_count;
  uint8_t select_index;
  UIState *state;
  uint8_t update_index;
} UIScreen;

UIScreen ui_screen_new(
  UIState *state,
  UIDisplay *display,
  UIField fields[8],
  uint8_t fields_count
);
uint8_t ui_selected_position(UIScreen *screen);
int16_t ui_selected_modifier(UIScreen *screen);
void ui_move_selected(UIScreen *screen, UIScreenDirection direction);
void ui_update_selected_modifier(UIScreen *screen, int8_t modifier);
void ui_update_selected_display(UIScreen *screen);
void ui_update_selected_state(UIScreen *screen);
UIDisplayChanged ui_is_display_changed(UIScreen *screen);
void ui_update_changed_display(UIScreen *screen);

#endif
