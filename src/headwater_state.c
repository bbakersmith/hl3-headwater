#include "stdint.h"

#include "headwater_state.h"

static uint8_t SECONDS_IN_MINUTE = 60;
static uint32_t SAMPLES_PER_SECOND = 1000;

// TODO enum
static int8_t CLOCK_RUNNING_STOP = 0;
static int8_t CLOCK_RUNNING_PLAY = 1;

HeadwaterState create_headwater_state() {
  HeadwaterState state = {
    .tbpm = 600,
    .running = 0, // TODO enum
    .multiplier = 4,
    .reset = 0, // TODO enum
    .samples_since_reset_count = 0,
    .samples_per_beat = 1000,
    .samples_per_multiplied = 250,
    .sample_count = 0,
    .multiplied_sample_count = 0,
    .multiplied_count = 0
  };
  return state;
}

uint16_t headwater_state_samples_per_beat(int16_t tbpm) {
  return (SAMPLES_PER_SECOND * SECONDS_IN_MINUTE * 10) / tbpm;
}

void headwater_state_update(
    HeadwaterState *state,
    int16_t tbpm,
    int8_t multiplier
) {
  state->tbpm = tbpm;
  state->multiplier = multiplier;

  uint16_t tbpm_samples = headwater_state_samples_per_beat(tbpm);
  state->samples_per_beat = tbpm_samples;
  state->samples_per_multiplied = tbpm_samples / multiplier;
}

void headwater_state_stop(HeadwaterState *state) {
  state->running = CLOCK_RUNNING_STOP;
}

void headwater_state_reset(HeadwaterState *state) {
  state->sample_count = 0;
  state->multiplied_sample_count = 0;
  state->multiplied_count = 0;
  state->samples_since_reset_count = 0;
}

void headwater_state_start(HeadwaterState *state) {
  state->sample_count = 0;
  state->multiplied_sample_count = 0;
  state->multiplied_count = 0;
  state->running = CLOCK_RUNNING_PLAY;
}

void headwater_state_increment_counts(HeadwaterState *state) {
  state->sample_count += 1;
  state->multiplied_sample_count += 1;
  // TODO prevent overflow, timeout reset
  state->samples_since_reset_count += 1;
  // TODO move this into update clock outputs to reduce branching?
  if(state->samples_per_beat <= state->sample_count) {
    state->sample_count = 0;
    state->multiplied_sample_count = 0;
    state->multiplied_count = 0;
  }
}

void update_outputs(
  HeadwaterState *state,
  HeadwaterOutputFn output_fn,
  HeadwaterOutputFn multiplied_fn
) {
  if(state->sample_count == 0) {
    output_fn(1);
    multiplied_fn(1);
    state->multiplied_count += 1;
  } else {
    output_fn(0);
    if(state->multiplied_count < state->multiplier &&
        state->samples_per_multiplied <= state->multiplied_sample_count) {

      state->multiplied_count += 1;
      state->multiplied_sample_count = 0;
      multiplied_fn(1);
    } else {
      multiplied_fn(0);
    }
  }
}

void headwater_state_cycle(
  HeadwaterState *state,
  HeadwaterOutputFn output_fn,
  HeadwaterOutputFn multiplied_fn
) {
  // TODO handle this at the interrupt level?
  // ex. play button enables TIMER1_COMPA interrupt, stop disables it
  if(state->running == CLOCK_RUNNING_PLAY) {
    update_outputs(state, output_fn, multiplied_fn);
    headwater_state_increment_counts(state);
  }
}
