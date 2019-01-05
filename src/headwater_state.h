#ifndef _HEADWATER_STATE_H_
#define _HEADWATER_STATE_H_

#include "stdint.h"

typedef struct HeadwaterState {
  uint16_t tbpm;
  uint8_t multiplier;
  uint8_t running;
  uint16_t reset;
  uint16_t samples_since_reset_count;
  uint16_t samples_per_beat;
  uint16_t samples_per_multiplied;
  uint16_t sample_count;
  uint16_t multiplied_sample_count;
  uint16_t multiplied_count;
  uint8_t output;
  uint8_t multiplied;
} HeadwaterState;

// TODO should be able to avoid dealing with this in state module
typedef void (HeadwaterOutputFn)(uint8_t enabled);

HeadwaterState create_headwater_state();

uint16_t headwater_state_samples_per_beat(int16_t tbpm);
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
