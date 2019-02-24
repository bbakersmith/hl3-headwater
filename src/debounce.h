#ifndef _DEBOUNCE_H_
#define _DEBOUNCE_H_

#include "stdint.h"

typedef enum {
  DEBOUNCE_BUTTON_CHANGE_FALSE,
  DEBOUNCE_BUTTON_CHANGE_TRUE
} DEBOUNCE_BUTTON_CHANGE;

typedef enum {
  DEBOUNCE_BUTTON_STATE_LOW,
  DEBOUNCE_BUTTON_STATE_HIGH,
} DEBOUNCE_BUTTON_STATE;

typedef enum {
  DEBOUNCE_RE_OUTPUT_NONE,
  DEBOUNCE_RE_OUTPUT_LEFT,
  DEBOUNCE_RE_OUTPUT_RIGHT
} DEBOUNCE_RE_OUTPUT;

typedef enum {
  DEBOUNCE_RE_EVENT_BOTH,
  DEBOUNCE_RE_EVENT_B,
  DEBOUNCE_RE_EVENT_A,
  DEBOUNCE_RE_EVENT_NONE
} DEBOUNCE_RE_EVENT;

typedef enum {
  DEBOUNCE_RE_STATE_NONE,
  DEBOUNCE_RE_STATE_LEFT_1,
  DEBOUNCE_RE_STATE_LEFT_2,
  DEBOUNCE_RE_STATE_LEFT_3,
  DEBOUNCE_RE_STATE_LEFT_OUTPUT,
  DEBOUNCE_RE_STATE_RIGHT_1,
  DEBOUNCE_RE_STATE_RIGHT_2,
  DEBOUNCE_RE_STATE_RIGHT_3,
  DEBOUNCE_RE_STATE_RIGHT_OUTPUT
} DEBOUNCE_RE_STATE;

static DEBOUNCE_RE_STATE DebounceRETransition[9][4] = {
  {
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_LEFT_1,
    DEBOUNCE_RE_STATE_RIGHT_1,
    DEBOUNCE_RE_STATE_NONE
  },
  {
    DEBOUNCE_RE_STATE_LEFT_2,
    DEBOUNCE_RE_STATE_LEFT_1,
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_NONE
  },
  {
    DEBOUNCE_RE_STATE_LEFT_2,
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_LEFT_3,
    DEBOUNCE_RE_STATE_NONE
  },
  {
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_LEFT_3,
    DEBOUNCE_RE_STATE_LEFT_OUTPUT
  },
  {
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_LEFT_1,
    DEBOUNCE_RE_STATE_RIGHT_1,
    DEBOUNCE_RE_STATE_NONE
  },
  {
    DEBOUNCE_RE_STATE_RIGHT_2,
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_RIGHT_1,
    DEBOUNCE_RE_STATE_NONE
  },
  {
    DEBOUNCE_RE_STATE_RIGHT_2,
    DEBOUNCE_RE_STATE_RIGHT_3,
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_NONE
  },
  {
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_RIGHT_3,
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_RIGHT_OUTPUT
  },
  {
    DEBOUNCE_RE_STATE_NONE,
    DEBOUNCE_RE_STATE_LEFT_1,
    DEBOUNCE_RE_STATE_RIGHT_1,
    DEBOUNCE_RE_STATE_NONE
  }
};

typedef struct DebounceButton {
  uint8_t change;
  // TODO this is run in main loop so timing is irregular
  uint8_t debounce_count;
  uint8_t debounce_threshold;
  // TODO hold_count?
  DEBOUNCE_BUTTON_STATE initial_state;
  DEBOUNCE_BUTTON_STATE state;
} DebounceButton;

typedef struct DebounceRE {
  DebounceButton a;
  DebounceButton b;
  DEBOUNCE_RE_OUTPUT output;
  DEBOUNCE_RE_STATE state;
} DebounceRE;

DebounceButton debounce_button_new(
  DEBOUNCE_BUTTON_STATE initial_state,
  uint8_t debounce_threshold
);

DebounceRE debounce_rotary_encoder_new(
  DEBOUNCE_BUTTON_STATE initial_state,
  uint8_t debounce_threshold
);

void debounce_button_update(
  DebounceButton *button,
  DEBOUNCE_BUTTON_STATE value
);

void debounce_button_reset(DebounceButton *button);

void debounce_rotary_encoder_update(
  DebounceRE *rotary_encoder,
  DEBOUNCE_BUTTON_STATE a_value,
  DEBOUNCE_BUTTON_STATE b_value
);

void debounce_re_reset(DebounceRE *rotary_encoder);

#endif
