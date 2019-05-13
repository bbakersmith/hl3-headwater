#ifndef _DEBOUNCE_H_
#define _DEBOUNCE_H_

#include "stdint.h"

typedef enum {
  DEBOUNCE_BUTTON_CHANGE_LOW,
  DEBOUNCE_BUTTON_CHANGE_HIGH,
  DEBOUNCE_BUTTON_CHANGE_NONE
} DebounceButtonChange;

typedef enum {
  DEBOUNCE_BUTTON_STATE_LOW,
  DEBOUNCE_BUTTON_STATE_HIGH,
} DebounceButtonState;

typedef enum {
  DEBOUNCE_ENCODER_OUTPUT_NONE,
  DEBOUNCE_ENCODER_OUTPUT_LEFT,
  DEBOUNCE_ENCODER_OUTPUT_RIGHT
} DebounceEncoderOutput;

typedef enum {
  DEBOUNCE_ENCODER_EVENT_BOTH,
  DEBOUNCE_ENCODER_EVENT_B,
  DEBOUNCE_ENCODER_EVENT_A,
  DEBOUNCE_ENCODER_EVENT_NONE
} DebounceEncoderEvent;

typedef enum {
  DEBOUNCE_ENCODER_STATE_NONE,
  DEBOUNCE_ENCODER_STATE_LEFT_1,
  DEBOUNCE_ENCODER_STATE_LEFT_2,
  DEBOUNCE_ENCODER_STATE_LEFT_3,
  DEBOUNCE_ENCODER_STATE_LEFT_OUTPUT,
  DEBOUNCE_ENCODER_STATE_RIGHT_1,
  DEBOUNCE_ENCODER_STATE_RIGHT_2,
  DEBOUNCE_ENCODER_STATE_RIGHT_3,
  DEBOUNCE_ENCODER_STATE_RIGHT_OUTPUT
} DebounceEncoderState;

typedef volatile struct DebounceButton {
  uint8_t change;
  uint8_t debounce_count;
  uint8_t debounce_threshold;
  uint16_t hold_count;
  DebounceButtonState initial_state;
  DebounceButtonState state;
} DebounceButton;

typedef volatile struct DebounceEncoder {
  DebounceButton a;
  DebounceButton b;
  DebounceEncoderOutput output;
  DebounceEncoderState state;
} DebounceEncoder;

DebounceButton debounce_button_new(
  DebounceButtonState initial_state,
  uint8_t debounce_threshold
);

DebounceEncoder debounce_encoder_new(
  DebounceButtonState initial_state,
  uint8_t debounce_threshold
);

void debounce_button_update(
  DebounceButton *button,
  DebounceButtonState value
);

void debounce_button_reset(DebounceButton *button);

void debounce_encoder_update(
  DebounceEncoder *encoder,
  DebounceButtonState a_value,
  DebounceButtonState b_value
);

void debounce_encoder_reset(DebounceEncoder *encoder);

#endif
