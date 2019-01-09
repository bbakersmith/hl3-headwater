#ifndef _HEADWATER_STATE_H_
#define _HEADWATER_STATE_H_

#include "stdint.h"

// TODO is there an abstraction for each of the channels here?
typedef struct HeadwaterState {
  uint16_t bpm;
  uint8_t multiplier_a;
  uint8_t multiplier_b;
  uint8_t running; // TODO rename?
  uint16_t samples_since_reset_count;
  uint16_t samples_per_bpm;
  uint16_t samples_per_multiplier_a;
  uint16_t samples_per_multiplier_b;
  uint16_t count_multiplier_a;
  uint16_t count_multiplier_b;
  uint16_t sample_count_bpm;
  uint16_t sample_count_multiplier_a;
  uint16_t sample_count_multiplier_b;
  uint8_t cv_bpm;
  uint8_t cv_multiplier_a;
  uint8_t cv_multiplier_b;
  uint8_t output_bpm;
  uint8_t output_multiplier_a;
  uint8_t output_multiplier_b;
  uint8_t change_flags;
  uint8_t mode;

  uint16_t reset; // TODO remove
  uint16_t tbpm; // TODO remove
  uint8_t multiplier; // TODO remove
  uint16_t samples_per_multiplied; // TODO remove
  uint16_t sample_count; // TODO remove
  uint16_t multiplied_sample_count; // TODO remove
  uint16_t multiplied_count; // TODO remove
  uint8_t output; // TODO remove
  uint8_t multiplied; // TODO remove
} HeadwaterState;

typedef enum {
  HEADWATER_STATE_CHANGE_BPM,
  HEADWATER_STATE_CHANGE_MULTIPLIER_A,
  HEADWATER_STATE_CHANGE_MULTIPLIER_B,
  HEADWATER_STATE_CHANGE_PLAY,
  HEADWATER_STATE_CHANGE_RESET,
  HEADWATER_STATE_CHANGE_STOP,
  HEADWATER_STATE_CHANGE_MODE
} HeadwaterStateChange;

// TODO should be able to avoid dealing with this in state module
typedef void (HeadwaterOutputFn)(uint8_t enabled);

HeadwaterState create_headwater_state();

uint16_t headwater_state_samples_per_bpm(int16_t tbpm);
void headwater_state_increment_counts(HeadwaterState *state);
void headwater_state_update(
  HeadwaterState *state,
  int16_t tbpm,
  int8_t multiplier
);
void headwater_state_stop(HeadwaterState *state);
void headwater_state_reset(HeadwaterState *state);
void headwater_state_start(HeadwaterState *state);
void headwater_state_cycle(
  HeadwaterState *state,
  HeadwaterOutputFn output_fn,
  HeadwaterOutputFn multiplied_fn
);

#endif
