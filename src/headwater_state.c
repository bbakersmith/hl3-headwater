#include "stdint.h"
#include <stdio.h>

#include "headwater_state.h"

HeadwaterStateChannel headwater_state_new_channel(uint16_t samples_per_beat) {
  HeadwaterStateChannel channel = {
    .samples_per_beat = samples_per_beat,
    .samples = 0,
    .beats = 0,
    .multiplier = 1,
    .output = 0
  };
  return channel;
}

HeadwaterState headwater_state_new(void) {
  uint32_t samples_per_second = 1000;

  uint16_t samples = headwater_state_bpm_to_samples(
    samples_per_second,
    HEADWATER_STATE_DEFAULT_BPM
  );

  HeadwaterStateChannel bpm_channel =
    headwater_state_new_channel(samples);

  HeadwaterStateChannel multiplier_a_channel =
    headwater_state_new_channel(samples);

  HeadwaterStateChannel multiplier_b_channel =
    headwater_state_new_channel(samples);

  HeadwaterState state = {
    .bpm = HEADWATER_STATE_DEFAULT_BPM,
    .bpm_channel = bpm_channel,
    .mode = HEADWATER_STATE_MODE_INTERNAL,
    .multiplier_a_channel = multiplier_a_channel,
    .multiplier_b_channel = multiplier_b_channel,
    .output_enabled = 0,
    .preset = 0,
    .reset_count = 0,
    .samples_per_second = samples_per_second,
    .samples_since_reset_count = 0
  };
  return state;
}

int16_t headwater_state_samples_to_bpm(
  uint32_t samples_per_second,
  uint16_t samples_per_beat
) {
  return (samples_per_second * SECONDS_IN_MINUTE * 10) / samples_per_beat;
}

uint16_t headwater_state_bpm_to_samples(
  uint32_t samples_per_second,
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

void headwater_state_reset_channel(HeadwaterStateChannel *channel) {
  channel->beats = 0;
  channel->samples = 0;
}

void headwater_state_play(HeadwaterState *state) {
  headwater_state_reset_channel(&state->bpm_channel);
  headwater_state_reset_channel(&state->multiplier_a_channel);
  headwater_state_reset_channel(&state->multiplier_b_channel);

  state->reset_count++;

  switch(state->mode) {
    case HEADWATER_STATE_MODE_EXTERNAL:
      if(state->reset_count == 1) {
        // TODO display unknown ---.- ???.?
      } else {
        state->output_enabled = 1; // TODO enum

        state->bpm_channel.samples_per_beat =
          state->samples_since_reset_count;

        state->change_flags |= (1 << HEADWATER_STATE_CHANGE_BPM);
      }
      state->samples_since_reset_count = 0;
      break;
    default:
      state->output_enabled = 1; // TODO enum
      break;
  }
}

void headwater_state_increment_counts(HeadwaterState *state) {
  if(state->bpm_channel.samples < UINT16_MAX)
    state->bpm_channel.samples++;

  if(state->multiplier_a_channel.samples < UINT16_MAX)
    state->multiplier_a_channel.samples++;

  if(state->multiplier_b_channel.samples < UINT16_MAX)
    state->multiplier_b_channel.samples++;

  // timeout & reset
  if(state->samples_since_reset_count < UINT16_MAX) {
    state->samples_since_reset_count++;
  } else {
    headwater_state_stop(state);
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
  } else {
    state->bpm_channel.output = 0;
    state->multiplier_a_channel.output = 0;
    state->multiplier_b_channel.output = 0;
  }
  headwater_state_increment_counts(state);
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
    headwater_state_stop(state);

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
