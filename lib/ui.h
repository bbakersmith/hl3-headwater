/**
 * @file ui.h
 */

#ifndef _UI_H_
#define _UI_H_

#include "stdbool.h"
#include "stdint.h"

#ifndef UIDisplay
#include "dummy_struct.h"
#define UIDisplay DummyStruct
#endif

#ifndef UIState
#include "dummy_struct.h"
#define UIState DummyStruct
#endif

/**
 * Direction to move field selection.
 */
typedef enum {
  UI_SCREEN_DIRECTION_DEC,
  UI_SCREEN_DIRECTION_INC
} UIScreenDirection;

/**
 * Metadata for a single field on the screen, including functions and
 * parameters necessary for updating the display and the represented state.
 */
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

/**
 * Metadata needed to update all the fields on the screen.
 */
typedef volatile struct UIScreen {
  uint8_t change_flags;
  UIDisplay *display;
  UIField fields[8];
  uint8_t fields_count;
  uint8_t select_index;
  UIState *state;
  uint8_t update_index;
} UIScreen;

/**
 * Create new UIScreen.
 */
UIScreen ui_screen_new(
  UIState *state,
  UIDisplay *display,
  UIField fields[8],
  uint8_t fields_count
);

/**
 * Return the position of the currently selected field.
 */
uint8_t ui_selected_position(UIScreen *screen);

/**
 * Return the uncommitted modifier of the currently selected field.
 */
int16_t ui_selected_modifier(UIScreen *screen);

/**
 * Move selection one field in the given direction.
 */
void ui_move_selected(UIScreen *screen, UIScreenDirection direction);

/**
 * Add or subtract from the uncommitted modifier for the currently selected
 * field.
 */
void ui_update_selected_modifier(UIScreen *screen, int8_t modifier);

/**
 * Update display for currently selected field, applying the uncommitted
 * modifier to the committed field value.
 */
void ui_update_selected_display(UIScreen *screen);

/**
 * Update the committed field value by applying the uncommitted modifier.
 */
void ui_update_selected_state(UIScreen *screen);

/**
 * TODO
 */
bool ui_is_display_changed(UIScreen *screen);

/**
 * TODO
 */
void ui_update_changed_display(UIScreen *screen);

#endif
