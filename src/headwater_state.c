#include "stdint.h"
#include <stdio.h>

#include "headwater_state.h"

#define SECONDS_IN_MINUTE 60

HeadwaterStateChannel headwater_state_new_channel(void) {
  HeadwaterStateChannel channel = {
    .samples_per_beat = 1000,
    .samples = 0,
    .beats = 0,
    .multiplier = 1,
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
    .bpm_channel = bpm_channel,
    .mode = HEADWATER_STATE_MODE_INTERNAL,
    .multiplier_a_channel = multiplier_a_channel,
    .multiplier_b_channel = multiplier_b_channel,
    .output_enabled = 0,
    .preset = 0,
    .reset_count = 0,
    .samples_per_second = 1000,
    .samples_since_reset_count = 0
  };
  return state;
}

int16_t headwater_state_samples_to_bpm(
  uint16_t samples_per_second,
  uint16_t samples_per_beat
) {
  return (samples_per_second * SECONDS_IN_MINUTE * 10) / samples_per_beat;
}

uint16_t headwater_state_bpm_to_samples(
  uint16_t samples_per_second,
  int16_t bpm
) {
  return (samples_per_second * SECONDS_IN_MINUTE * 10) / bpm;
}

void headwater_state_update_samples_per_beat(
  HeadwaterStateChannel *channel,
  uint16_t samples_per_beat
) {
  channel->samples_per_beat = samples_per_beat / channel->multiplier;
}

void headwater_state_stop(HeadwaterState *state) {
  state->output_enabled = 0; // TODO enum
  state->reset_count = 0;
}

void headwater_state_play(HeadwaterState *state) {
  state->bpm_channel.samples = 0;
  state->multiplier_a_channel.samples = 0;
  state->multiplier_a_channel.beats = 0;
  state->multiplier_b_channel.samples = 0;
  state->multiplier_b_channel.beats = 0;
  state->output_enabled = 1; // TODO enum
  state->reset_count++;
}

void headwater_state_increment_counts(HeadwaterState *state) {
  state->bpm_channel.samples += 1;
  state->multiplier_a_channel.samples += 1;
  state->multiplier_b_channel.samples += 1;

  // timeout & reset at overflow
  if(state->samples_since_reset_count < 65535) {
    state->samples_since_reset_count += 1;
  } else {
    state->samples_since_reset_count = 0;
    state->reset_count = 0;
  }

  // loop based on internal clock for internal and tap modes
  if(
    state->mode == HEADWATER_STATE_MODE_INTERNAL
    || state->mode == HEADWATER_STATE_MODE_TAP
  ) {
    if(state->bpm_channel.samples_per_beat <= state->bpm_channel.samples) {
      state->bpm_channel.samples = 0;
      state->multiplier_a_channel.samples = 0;
      state->multiplier_b_channel.samples = 0;

      state->bpm_channel.beats = 0;
      state->multiplier_a_channel.beats = 0;
      state->multiplier_b_channel.beats = 0;
    }
  }
}

void headwater_state_update_outputs(HeadwaterState *state) {
  if(state->bpm_channel.samples == 0) {
    state->bpm_channel.output = 1;
    state->multiplier_a_channel.output = 1;
    state->multiplier_a_channel.beats += 1;
    state->multiplier_b_channel.output = 1;
    state->multiplier_b_channel.beats += 1;

    switch(state->mode) {
      case HEADWATER_STATE_MODE_EXTERNAL:
        if(1 < state->reset_count) {
          state->bpm_channel.samples_per_beat =
            state->samples_since_reset_count;

          state->samples_since_reset_count = 0;
          state->change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
        }
        break;
      default:
        break;
    }
  } else {
    state->bpm_channel.output = 0;

    if(
      state->multiplier_a_channel.beats
      < state->multiplier_a_channel.multiplier
      && state->multiplier_a_channel.samples_per_beat
      <= state->multiplier_a_channel.samples
    ) {
      state->multiplier_a_channel.output = 1;
      state->multiplier_a_channel.beats += 1;
      state->multiplier_a_channel.samples = 0;
    } else {
      state->multiplier_a_channel.output = 0;
    }

    if(
      state->multiplier_b_channel.beats
      < state->multiplier_b_channel.multiplier
      && state->multiplier_b_channel.samples_per_beat
      <= state->multiplier_b_channel.samples
    ) {
      state->multiplier_b_channel.output = 1;
      state->multiplier_b_channel.beats += 1;
      state->multiplier_b_channel.samples = 0;
    } else {
      state->multiplier_b_channel.output = 0;
    }
  }
}

void headwater_state_cycle(HeadwaterState *state) {
  if(state->output_enabled == 1) {
    headwater_state_update_outputs(state);
    headwater_state_increment_counts(state);
  } else {
    state->bpm_channel.output = 0;
    state->multiplier_a_channel.output = 0;
    state->multiplier_b_channel.output = 0;
    state->samples_since_reset_count = 0;
    state->reset_count = 0;
  }
}

void headwater_state_handle_change(HeadwaterState *state) {
  uint8_t change_flags = state->change_flags;

  if((change_flags & (1 << HEADWATER_STATE_CHANGE_PLAY))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_PLAY);
    headwater_state_play(state);

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

    switch(state->mode) {
      case HEADWATER_STATE_MODE_INTERNAL:
        headwater_state_update_samples_per_beat(
          &state->bpm_channel,
          headwater_state_bpm_to_samples(
            state->samples_per_second,
            state->bpm
          )
        );
        break;

      case HEADWATER_STATE_MODE_EXTERNAL:
        state->bpm = headwater_state_samples_to_bpm(
          state->samples_per_second,
          state->bpm_channel.samples_per_beat
        );
        break;

      default:
        break;
    }

    headwater_state_update_samples_per_beat(
      &state->multiplier_a_channel,
      state->bpm_channel.samples_per_beat
    );

    headwater_state_update_samples_per_beat(
      &state->multiplier_b_channel,
      state->bpm_channel.samples_per_beat
    );

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A);
    headwater_state_update_samples_per_beat(
      &state->multiplier_a_channel,
      state->bpm_channel.samples_per_beat
    );

  } else if((change_flags & (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B))) {
    state->change_flags &= ~(1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B);
    headwater_state_update_samples_per_beat(
      &state->multiplier_b_channel,
      state->bpm_channel.samples_per_beat
    );
  }
}
