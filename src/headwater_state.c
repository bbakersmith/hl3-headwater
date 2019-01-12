#include "stdint.h"
#include <stdio.h>

#include "headwater_state.h"

static uint8_t SECONDS_IN_MINUTE = 60;
static uint32_t SAMPLES_PER_SECOND = 1000;

// TODO enum
static int8_t CLOCK_RUNNING_STOP = 0;
static int8_t CLOCK_RUNNING_PLAY = 1;

HeadwaterStateChannel headwater_state_new_channel(void) {
  HeadwaterStateChannel channel = {
    .samples_per_beat = 1000,
    .samples = 0,
    .beats = 0,
    .modifier = 255,
    .output = 0
  };
  return channel;
}

HeadwaterState headwater_state_new(void) {
  HeadwaterStateChannel bpm_channel = headwater_state_new_channel();
  HeadwaterStateChannel multiplier_a_channel = headwater_state_new_channel();
  HeadwaterStateChannel multiplier_b_channel = headwater_state_new_channel();

  HeadwaterState state = {
    .bpm = 600,
    .multiplier_a = 4,
    .reset = 0, // TODO enum
    .samples_since_reset_count = 0,
    .bpm_channel = bpm_channel,
    .multiplier_a_channel = multiplier_a_channel,
    .multiplier_b_channel = multiplier_b_channel
  };
  return state;
}

uint16_t headwater_state_samples_per_bpm(int16_t bpm) {
  return (SAMPLES_PER_SECOND * SECONDS_IN_MINUTE * 10) / bpm;
}

// TODO separate out bpm and multiplier recalculations
void headwater_state_update(
  HeadwaterState *state,
  int16_t bpm,
  int8_t multiplier_a
) {
  state->bpm = bpm;
  state->multiplier_a = multiplier_a;

  uint16_t bpm_samples = headwater_state_samples_per_bpm(bpm);
  state->bpm_channel.samples_per_beat = bpm_samples;
  state->multiplier_a_channel.samples_per_beat = bpm_samples / multiplier_a;
}

void headwater_state_stop(HeadwaterState *state) {
  state->output_enabled = 0; // TODO enum
}

void headwater_state_reset(HeadwaterState *state) {
  state->bpm_channel.samples = 0;
  state->multiplier_a_channel.samples = 0;
  state->multiplier_a_channel.beats = 0;
  state->samples_since_reset_count = 0;
}

// TODO rename start > play
void headwater_state_start(HeadwaterState *state) {
  state->bpm_channel.samples = 0;
  state->multiplier_a_channel.samples = 0;
  state->multiplier_a_channel.beats = 0;
  state->output_enabled = 1; // TODO enum
}

void headwater_state_increment_counts(HeadwaterState *state) {
  state->bpm_channel.samples += 1;
  state->multiplier_a_channel.samples += 1;

  // TODO prevent overflow, timeout reset
  state->samples_since_reset_count += 1;

  if(state->bpm_channel.samples_per_beat <= state->bpm_channel.samples) {
    state->bpm_channel.samples = 0;
    state->multiplier_a_channel.samples = 0;

    state->bpm_channel.beats = 0;
    state->multiplier_a_channel.beats = 0;
  }
}

void headwater_state_update_outputs(HeadwaterState *state) {
  if(state->output_enabled == 1) {
    if(state->bpm_channel.samples == 0) {
      state->bpm_channel.output = 1;
      state->multiplier_a_channel.output = 1;
      state->multiplier_a_channel.beats += 1;
    } else {
      state->bpm_channel.output = 0;
      if(state->multiplier_a_channel.beats < state->multiplier_a
          && state->multiplier_a_channel.samples_per_beat
          <= state->multiplier_a_channel.samples) {

        state->multiplier_a_channel.output = 1;
        state->multiplier_a_channel.beats += 1;
        state->multiplier_a_channel.samples = 0;
      } else {
        state->multiplier_a_channel.output = 0;
      }
    }
  } else {
    state->bpm_channel.output = 0;
    state->multiplier_a_channel.output = 0;
  }
}

void headwater_state_cycle(HeadwaterState *state) {
  headwater_state_update_outputs(state);
  headwater_state_increment_counts(state);
}

void headwater_state_change(HeadwaterState *state) {
  uint8_t change_flags = state->change_flags;

  if((change_flags & (1 << HEADWATER_STATE_CHANGE_PLAY))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_PLAY);
    headwater_state_start(state);

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_RESET))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_RESET);
    headwater_state_reset(state);

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_STOP))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_STOP);
    headwater_state_stop(state);

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MODE))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MODE);
    // TODO react to mode change

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_BPM))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_BPM);
    // TODO update bpm
    // TODO update multiplier a
    // TODO update multiplier b

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
    // TODO update multiplier a

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B);
    // TODO update multiplier b

  }
}
