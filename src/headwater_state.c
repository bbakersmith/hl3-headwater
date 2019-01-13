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
    .multiplier = 1,
    .output = 0,
    .output_enabled = 1
  };
  return channel;
}

HeadwaterState headwater_state_new(void) {
  HeadwaterStateChannel bpm_channel = headwater_state_new_channel();
  HeadwaterStateChannel multiplier_a_channel = headwater_state_new_channel();
  HeadwaterStateChannel multiplier_b_channel = headwater_state_new_channel();

  HeadwaterState state = {
    .bpm = 600,
    .samples_per_beat = 1000,
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

uint16_t headwater_state_apply_modifier(uint16_t value, uint8_t modifier) {
  return (value * (25500 / modifier)) / 100;
}

void headwater_state_update_samples_per_beat(
  HeadwaterStateChannel *channel,
  uint16_t samples_per_beat
) {
  if(channel->modifier == 0) {
    // TODO this should really be "running" or something like that
    // - how should counts be handled when modifier is 0???
    // - when it resumes???
    channel->output_enabled = 0; // TODO enum
  } else {
    uint16_t modified = headwater_state_apply_modifier(
      samples_per_beat,
      channel->modifier
    );
    uint16_t multiplied = modified / channel->multiplier;
    channel->samples_per_beat = multiplied;
    channel->output_enabled = 1; // TODO enum
  }
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
void headwater_state_play(HeadwaterState *state) {
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
      if(state->multiplier_a_channel.beats
          < state->multiplier_a_channel.multiplier
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
    headwater_state_play(state);

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
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B);

    // TODO this should be its own type of change, apart from bpm channel
    state->samples_per_beat = headwater_state_samples_per_bpm(state->bpm);

    headwater_state_update_samples_per_beat(
      &state->bpm_channel,
      state->samples_per_beat
    );

    headwater_state_update_samples_per_beat(
      &state->multiplier_a_channel,
      state->bpm_channel.samples_per_beat
    );

    // TODO multiplier b

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
    headwater_state_update_samples_per_beat(
      &state->multiplier_a_channel,
      state->bpm_channel.samples_per_beat
    );

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B);
    // TODO multiplier b

  }
}
