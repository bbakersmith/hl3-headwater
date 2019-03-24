#ifndef _DEBOUNCE_H_
#define _DEBOUNCE_H_

#include "stdint.h"

typedef enum {
  DEBOUNCE_BUTTON_CHANGE_LOW,
  DEBOUNCE_BUTTON_CHANGE_HIGH,
  DEBOUNCE_BUTTON_CHANGE_NONE
} DEBOUNCE_BUTTON_CHANGE;

typedef enum {
  DEBOUNCE_BUTTON_STATE_LOW,
  DEBOUNCE_BUTTON_STATE_HIGH,
} DEBOUNCE_BUTTON_STATE;

typedef enum {
  DEBOUNCE_ENCODER_OUTPUT_NONE,
  DEBOUNCE_ENCODER_OUTPUT_LEFT,
  DEBOUNCE_ENCODER_OUTPUT_RIGHT
} DEBOUNCE_ENCODER_OUTPUT;

typedef enum {
  DEBOUNCE_ENCODER_EVENT_BOTH,
  DEBOUNCE_ENCODER_EVENT_B,
  DEBOUNCE_ENCODER_EVENT_A,
  DEBOUNCE_ENCODER_EVENT_NONE
} DEBOUNCE_ENCODER_EVENT;

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
} DEBOUNCE_ENCODER_STATE;

typedef volatile struct DebounceButton {
  uint8_t change;
  uint8_t debounce_count;
  uint8_t debounce_threshold;
  uint16_t hold_count;
  DEBOUNCE_BUTTON_STATE initial_state;
  DEBOUNCE_BUTTON_STATE state;
} DebounceButton;

typedef volatile struct DebounceEncoder {
  DebounceButton a;
  DebounceButton b;
  DEBOUNCE_ENCODER_OUTPUT output;
  DEBOUNCE_ENCODER_STATE state;
} DebounceEncoder;

DebounceButton debounce_button_new(
  DEBOUNCE_BUTTON_STATE initial_state,
  uint8_t debounce_threshold
);

DebounceEncoder debounce_encoder_new(
  DEBOUNCE_BUTTON_STATE initial_state,
  uint8_t debounce_threshold
);

void debounce_button_update(
  DebounceButton *button,
  DEBOUNCE_BUTTON_STATE value
);

void debounce_button_reset(DebounceButton *button);

void debounce_encoder_update(
  DebounceEncoder *encoder,
  DEBOUNCE_BUTTON_STATE a_value,
  DEBOUNCE_BUTTON_STATE b_value
);

void debounce_encoder_reset(DebounceEncoder *encoder);

#endif
