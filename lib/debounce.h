/**
 * @file debounce.h
 *
 * @brief Debounce button and rotary encoder inputs.
 */

#ifndef _DEBOUNCE_H_
#define _DEBOUNCE_H_

#include "stdint.h"

/**
 * Button output change event.
 */
typedef enum {
  DEBOUNCE_BUTTON_CHANGE_LOW,
  DEBOUNCE_BUTTON_CHANGE_HIGH,
  DEBOUNCE_BUTTON_CHANGE_NONE
} DebounceButtonChange;

/**
 * Button high or low state.
 */
typedef enum {
  DEBOUNCE_BUTTON_STATE_LOW,
  DEBOUNCE_BUTTON_STATE_HIGH,
} DebounceButtonState;

/**
 * Encoder output change event.
 */
typedef enum {
  DEBOUNCE_ENCODER_CHANGE_NONE,
  DEBOUNCE_ENCODER_CHANGE_LEFT,
  DEBOUNCE_ENCODER_CHANGE_RIGHT
} DebounceEncoderChange;

/**
 * Encoder transition event.
 */
typedef enum {
  DEBOUNCE_ENCODER_EVENT_BOTH,
  DEBOUNCE_ENCODER_EVENT_B,
  DEBOUNCE_ENCODER_EVENT_A,
  DEBOUNCE_ENCODER_EVENT_NONE
} DebounceEncoderEvent;

/**
 * Encoder states.
 */
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

/**
 * Metadata for button state.
 */
typedef volatile struct DebounceButton {
  uint8_t change;
  uint8_t debounce_count;
  uint8_t debounce_threshold;
  uint16_t hold_count;
  DebounceButtonState initial_state;
  DebounceButtonState state;
} DebounceButton;

/**
 * Metadata for encoder state.
 */
typedef volatile struct DebounceEncoder {
  DebounceButton a;
  DebounceButton b;
  DebounceEncoderChange output;
  DebounceEncoderState state;
} DebounceEncoder;

/**
 * Create new DebounceButton with given threshold and starting state.
 */
DebounceButton debounce_button_new(
  DebounceButtonState initial_state,
  uint8_t debounce_threshold
);

/**
 * Create new DebounceEncoder with given threshold and starting state.
 */
DebounceEncoder debounce_encoder_new(
  DebounceButtonState initial_state,
  uint8_t debounce_threshold
);

/**
 * Debounce sampled button input.
 */
void debounce_button_update(
  DebounceButton *button,
  DebounceButtonState value
);

/**
 * Reset button to initial state.
 */
void debounce_button_reset(DebounceButton *button);

/**
 * Debounce sampled encoder input.
 */
void debounce_encoder_update(
  DebounceEncoder *encoder,
  DebounceButtonState a_value,
  DebounceButtonState b_value
);

/**
 * Reset encoder to initial state.
 */
void debounce_encoder_reset(DebounceEncoder *encoder);

#endif
