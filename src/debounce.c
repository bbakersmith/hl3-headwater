#include "debounce.h"

// TODO DEBUG
// #include <stdio.h>

DebounceButton debounce_button_new(
  DEBOUNCE_BUTTON_STATE initial_state,
  uint8_t debounce_threshold
) {
  DebounceButton button = {
    .debounce_count = 0,
    .debounce_threshold = debounce_threshold,
    .initial_state = initial_state,
    .state = initial_state
  };
  return button;
}

DebounceRE debounce_rotary_encoder_new(
  DEBOUNCE_BUTTON_STATE initial_state,
  uint8_t debounce_threshold
) {
  DebounceRE rotary_encoder = {
    .a = debounce_button_new(initial_state, debounce_threshold),
    .b = debounce_button_new(initial_state, debounce_threshold),
    .output = DEBOUNCE_RE_OUTPUT_NONE,
    .state = DEBOUNCE_RE_STATE_NONE
  };
  return rotary_encoder;
}

void debounce_button_update(
  DebounceButton *button,
  DEBOUNCE_BUTTON_STATE new_state
) {
  if(button->state != new_state) {
    if(button->debounce_count < button->debounce_threshold) {
      button->debounce_count += 1;
      button->change = DEBOUNCE_BUTTON_CHANGE_FALSE;
    } else {
      button->debounce_count = 0;
      button->state = new_state;
      button->change = DEBOUNCE_BUTTON_CHANGE_TRUE;
    }
  } else {
    button->debounce_count = 0;
    button->change = DEBOUNCE_BUTTON_CHANGE_FALSE;
  }
}

void debounce_button_reset(DebounceButton *button) {
  if(button->state != button->initial_state) {
    button->state = button->initial_state;
    button->change = DEBOUNCE_BUTTON_CHANGE_TRUE;
  } else {
    button->change = DEBOUNCE_BUTTON_CHANGE_FALSE;
  }
}

// FIXME assumes rotary encoder is pulling to ground
void debounce_rotary_encoder_update(
  DebounceRE *rotary_encoder,
  DEBOUNCE_BUTTON_STATE a_value,
  DEBOUNCE_BUTTON_STATE b_value
) {
  debounce_button_update(&rotary_encoder->a, a_value);
  debounce_button_update(&rotary_encoder->b, b_value);

  DEBOUNCE_RE_EVENT debounce_event =
    rotary_encoder->a.state
    | (rotary_encoder->b.state << 1);

  rotary_encoder->state =
    DebounceRETransition[rotary_encoder->state][debounce_event];

  switch(rotary_encoder->state) {
    case DEBOUNCE_RE_STATE_LEFT_OUTPUT:
      rotary_encoder->output = DEBOUNCE_RE_OUTPUT_LEFT;
      rotary_encoder->state = DEBOUNCE_RE_STATE_NONE;
      break;
    case DEBOUNCE_RE_STATE_RIGHT_OUTPUT:
      rotary_encoder->output = DEBOUNCE_RE_OUTPUT_RIGHT;
      rotary_encoder->state = DEBOUNCE_RE_STATE_NONE;
      break;
    default:
      rotary_encoder->output = DEBOUNCE_RE_OUTPUT_NONE;
  }

  if(rotary_encoder->state == DEBOUNCE_RE_STATE_NONE) {
    debounce_re_reset(rotary_encoder);
  }
}

void debounce_re_reset(DebounceRE *rotary_encoder) {
  debounce_button_reset(&rotary_encoder->a);
  debounce_button_reset(&rotary_encoder->b);
}
