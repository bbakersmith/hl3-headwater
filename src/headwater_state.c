#include "stdint.h"

#include "headwater_state.h"

static uint8_t SECONDS_IN_MINUTE = 60;
static uint32_t SAMPLES_PER_SECOND = 1000;

// TODO enum
static int8_t CLOCK_RUNNING_STOP = 0;
static int8_t CLOCK_RUNNING_PLAY = 1;

HeadwaterState headwater_state_new() {
  HeadwaterState state = {
    .tbpm = 600,
    .running = 0, // TODO enum
    .multiplier = 4,
    .reset = 0, // TODO enum
    .samples_since_reset_count = 0,
    .samples_per_bpm = 1000,
    .samples_per_multiplied = 250,
    .sample_count = 0,
    .multiplied_sample_count = 0,
    .multiplied_count = 0
  };
  return state;
}

uint16_t headwater_state_samples_per_bpm(int16_t tbpm) {
  return (SAMPLES_PER_SECOND * SECONDS_IN_MINUTE * 10) / tbpm;
}

// TODO separate out bpm and multiplier recalculations
void headwater_state_update(
  HeadwaterState *state,
  int16_t tbpm,
  int8_t multiplier
) {
  state->tbpm = tbpm;
  state->multiplier = multiplier;

  uint16_t tbpm_samples = headwater_state_samples_per_bpm(tbpm);
  state->samples_per_bpm = tbpm_samples;
  state->samples_per_multiplied = tbpm_samples / multiplier;
}

void headwater_state_stop(HeadwaterState *state) {
  state->running = CLOCK_RUNNING_STOP; // TODO rename
}

void headwater_state_reset(HeadwaterState *state) {
  state->sample_count = 0;
  state->multiplied_sample_count = 0;
  state->multiplied_count = 0;
  state->samples_since_reset_count = 0;
}

// TODO rename start > play
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

  if(state->samples_per_bpm <= state->sample_count) {
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
  // TODO rename CLOCK_*
  if(state->running == CLOCK_RUNNING_PLAY) {
    update_outputs(state, output_fn, multiplied_fn);
    headwater_state_increment_counts(state);
  }
}

// TODO read and apply cv if state_change is 0

void headwater_state_change(HeadwaterState *state) {
  uint8_t change_flags = state->change_flags;

  if((change_flags & (1 << HEADWATER_STATE_CHANGE_PLAY))) {
    change_flags &= ~(1 << HEADWATER_STATE_CHANGE_PLAY);
    headwater_state_start(state);

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_RESET))) {
    change_flags &= ~(1 << HEADWATER_STATE_CHANGE_RESET);
    headwater_state_reset(state);

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_STOP))) {
    change_flags &= ~(1 << HEADWATER_STATE_CHANGE_STOP);
    headwater_state_stop(state);

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MODE))) {
    change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MODE);
    // TODO react to mode change

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_BPM))) {
    change_flags &= ~(1 << HEADWATER_STATE_CHANGE_BPM);
    // TODO update bpm
    // TODO update multiplier a
    // TODO update multiplier b

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A))) {
    change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
    // TODO update multiplier a

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B))) {
    change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B);
    // TODO update multiplier b

  }
}
