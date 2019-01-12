#ifndef _HEADWATER_STATE_H_
#define _HEADWATER_STATE_H_

#include "stdint.h"

typedef struct HeadwaterStateChannel {
  uint16_t samples_per_beat; // samples_per_multiplier_a
  uint16_t samples; // sample_count_multiplier_a
  uint16_t limit; // max beats
  uint16_t beats; // count_multiplier_a
  uint8_t modifier; // cv_multiplier_a
  uint8_t output; // output_multiplier_a
} HeadwaterStateChannel;

// TODO is there an abstraction for each of the channels here?
typedef struct HeadwaterState {
  uint8_t mode;
  uint8_t output_enabled;
  uint16_t bpm;
  uint8_t multiplier_a;
  uint8_t multiplier_b;
  HeadwaterStateChannel bpm_channel;
  HeadwaterStateChannel multiplier_a_channel;
  HeadwaterStateChannel multiplier_b_channel; // TODO test!
  uint8_t change_flags;

  uint16_t samples_since_reset_count; // TODO used?

  uint16_t reset; // TODO remove
} HeadwaterState;

typedef enum {
  HEADWATER_STATE_CHANGE_PLAY,
  HEADWATER_STATE_CHANGE_RESET,
  HEADWATER_STATE_CHANGE_STOP,
  HEADWATER_STATE_CHANGE_MODE,
  HEADWATER_STATE_CHANGE_BPM,
  HEADWATER_STATE_CHANGE_MULTIPLIER_A,
  HEADWATER_STATE_CHANGE_MULTIPLIER_B
} HEADWATER_STATE_CHANGE;

// TODO should be able to avoid dealing with this in state module
typedef void (HeadwaterOutputFn)(uint8_t enabled);

HeadwaterState headwater_state_new(void);
uint16_t headwater_state_samples_per_bpm(int16_t bpm);
void headwater_state_increment_counts(HeadwaterState *state);
void headwater_state_update(
  HeadwaterState *state,
  int16_t bpm,
  int8_t multiplier
);
void headwater_state_stop(HeadwaterState *state);
void headwater_state_reset(HeadwaterState *state);
void headwater_state_start(HeadwaterState *state);
void headwater_state_cycle(HeadwaterState *state);
void headwater_state_change(HeadwaterState *state);

#endif
