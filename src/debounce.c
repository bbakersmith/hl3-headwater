#include "debounce.h"

static DEBOUNCE_ENCODER_STATE DebounceEncoderTransition[9][4] = {
  {
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_LEFT_1,
    DEBOUNCE_ENCODER_STATE_RIGHT_1,
    DEBOUNCE_ENCODER_STATE_NONE
  },
  {
    DEBOUNCE_ENCODER_STATE_LEFT_2,
    DEBOUNCE_ENCODER_STATE_LEFT_1,
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_NONE
  },
  {
    DEBOUNCE_ENCODER_STATE_LEFT_2,
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_LEFT_3,
    DEBOUNCE_ENCODER_STATE_NONE
  },
  {
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_LEFT_3,
    DEBOUNCE_ENCODER_STATE_LEFT_OUTPUT
  },
  {
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_LEFT_1,
    DEBOUNCE_ENCODER_STATE_RIGHT_1,
    DEBOUNCE_ENCODER_STATE_NONE
  },
  {
    DEBOUNCE_ENCODER_STATE_RIGHT_2,
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_RIGHT_1,
    DEBOUNCE_ENCODER_STATE_NONE
  },
  {
    DEBOUNCE_ENCODER_STATE_RIGHT_2,
    DEBOUNCE_ENCODER_STATE_RIGHT_3,
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_NONE
  },
  {
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_RIGHT_3,
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_RIGHT_OUTPUT
  },
  {
    DEBOUNCE_ENCODER_STATE_NONE,
    DEBOUNCE_ENCODER_STATE_LEFT_1,
    DEBOUNCE_ENCODER_STATE_RIGHT_1,
    DEBOUNCE_ENCODER_STATE_NONE
  }
};

DebounceButton debounce_button_new(
  DEBOUNCE_BUTTON_STATE initial_state,
  uint8_t debounce_threshold
) {
  DebounceButton button = {
    .debounce_count = 0,
    .debounce_threshold = debounce_threshold,
    .hold_count = 0,
    .initial_state = initial_state,
    .state = initial_state
  };
  return button;
}

DebounceEncoder debounce_encoder_new(
  DEBOUNCE_BUTTON_STATE initial_state,
  uint8_t debounce_threshold
) {
  DebounceEncoder encoder = {
    .a = debounce_button_new(initial_state, debounce_threshold),
    .b = debounce_button_new(initial_state, debounce_threshold),
    .output = DEBOUNCE_ENCODER_OUTPUT_NONE,
    .state = DEBOUNCE_ENCODER_STATE_NONE
  };
  return encoder;
}

void debounce_button_update(
  DebounceButton *button,
  DEBOUNCE_BUTTON_STATE new_state
) {
  // TODO can remove hold count, not needed
  if(button->hold_count != 65535) {
    button->hold_count += 1;
  }
  if(button->state != new_state) {
    if(button->debounce_count < button->debounce_threshold) {
      button->debounce_count += 1;
      if(button->change != DEBOUNCE_BUTTON_CHANGE_NONE) {
        button->hold_count = 0;
        button->change = DEBOUNCE_BUTTON_CHANGE_NONE;
      }
    } else {
      button->debounce_count = 0;
      button->state = new_state;
      button->change = button->state;
    }
  } else {
    button->debounce_count = 0;
    if(button->change != DEBOUNCE_BUTTON_CHANGE_NONE) {
      button->hold_count = 0;
      button->change = DEBOUNCE_BUTTON_CHANGE_NONE;
    }
  }
}

void debounce_button_reset(DebounceButton *button) {
  if(button->state != button->initial_state) {
    button->state = button->initial_state;
    button->change = button->state;
  } else {
    button->change = DEBOUNCE_BUTTON_CHANGE_NONE;
  }
}

// FIXME assumes rotary encoder is pulling to ground
void debounce_encoder_update(
  DebounceEncoder *encoder,
  DEBOUNCE_BUTTON_STATE a_value,
  DEBOUNCE_BUTTON_STATE b_value
) {
  debounce_button_update(&encoder->a, a_value);
  debounce_button_update(&encoder->b, b_value);

  DEBOUNCE_ENCODER_EVENT debounce_event =
    encoder->a.state
    | (encoder->b.state << 1);

  encoder->state = DebounceEncoderTransition[encoder->state][debounce_event];

  switch(encoder->state) {
    case DEBOUNCE_ENCODER_STATE_LEFT_OUTPUT:
      encoder->output = DEBOUNCE_ENCODER_OUTPUT_LEFT;
      encoder->state = DEBOUNCE_ENCODER_STATE_NONE;
      break;
    case DEBOUNCE_ENCODER_STATE_RIGHT_OUTPUT:
      encoder->output = DEBOUNCE_ENCODER_OUTPUT_RIGHT;
      encoder->state = DEBOUNCE_ENCODER_STATE_NONE;
      break;
    default:
      encoder->output = DEBOUNCE_ENCODER_OUTPUT_NONE;
  }

  if(encoder->state == DEBOUNCE_ENCODER_STATE_NONE) {
    debounce_encoder_reset(encoder);
  }
}

void debounce_encoder_reset(DebounceEncoder *encoder) {
  debounce_button_reset(&encoder->a);
  debounce_button_reset(&encoder->b);
}
