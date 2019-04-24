#include "stdint.h"
#include <stdio.h>

#include "headwater_state.h"

void headwater_state_update_samples_per_beat(
  HeadwaterStateChannel *channel,
  uint16_t samples_per_beat
) {
  if(channel->multiplier == 0) {
    channel->output_enabled = 0;
    return;
  } else {
    channel->output_enabled = 1;
  }

  uint8_t multiplier = channel->multiplier;
  uint16_t base_samples = samples_per_beat / multiplier;
  uint8_t remainder = samples_per_beat % multiplier;
  uint16_t multiplier_100 = multiplier * 100;

  uint16_t no_remainder_frequency;
  uint16_t remainder_frequency;
  if(remainder != 0) {
    no_remainder_frequency = multiplier_100 / (multiplier - remainder);
    remainder_frequency = multiplier_100 / remainder;
  } else {
    no_remainder_frequency = 0;
    remainder_frequency = 1;
  }

  channel->samples_per_beat = base_samples;
  channel->samples_per_beat_with_remainder = base_samples;

  channel->remainder_frequency = remainder_frequency;
  channel->remainder_priority = remainder_frequency;

  channel->no_remainder_frequency = no_remainder_frequency;
  channel->no_remainder_priority = no_remainder_frequency;
}

HeadwaterStateChannel headwater_state_channel_new(uint16_t samples_per_beat) {
  HeadwaterStateChannel channel = {
    .samples = 0,
    .beats = 0,
    .multiplier = 1,
    .output = 0
  };
  headwater_state_update_samples_per_beat(&channel, samples_per_beat);
  return channel;
}

HeadwaterState headwater_state_new(void) {
  uint32_t samples_per_second = 1000;

  uint16_t samples_per_beat = headwater_state_bpm_to_samples(
    samples_per_second,
    HEADWATER_STATE_DEFAULT_BPM
  );

  HeadwaterStateChannel bpm_channel =
    headwater_state_channel_new(samples_per_beat);

  HeadwaterStateChannel multiplier_a_channel =
    headwater_state_channel_new(samples_per_beat);

  HeadwaterStateChannel multiplier_b_channel =
    headwater_state_channel_new(samples_per_beat);

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

uint16_t headwater_state_samples_to_bpm(
  uint32_t samples_per_second,
  uint16_t samples_per_beat
) {
  return (samples_per_second * SECONDS_IN_MINUTE * 10) / samples_per_beat;
}

uint16_t headwater_state_bpm_to_samples(
  uint32_t samples_per_second,
  uint16_t bpm
) {
  return (samples_per_second * SECONDS_IN_MINUTE * 10) / bpm;
}

void headwater_state_stop(HeadwaterState *state) {
  state->output_enabled = 0; // TODO enum
  state->reset_count = 0;
}

void headwater_state_reset_channel(HeadwaterStateChannel *channel) {
  channel->beats = 0;
  channel->samples = 0;
  channel->samples_per_beat_with_remainder = channel->samples_per_beat;
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

void headwater_state_channel_fire(
  HeadwaterStateChannel *channel
) {
  channel->samples_per_beat_with_remainder = channel->samples_per_beat;

  if(channel->remainder_priority < channel->no_remainder_priority) {
    channel->samples_per_beat_with_remainder++;
    channel->remainder_priority += channel->remainder_frequency;
  } else {
    channel->no_remainder_priority += channel->no_remainder_frequency;
  }

  // TODO prevent priority overflow (write test first)

  channel->beats++;
  channel->output = 1;
}

void headwater_state_channel_update_output(
  HeadwaterStateChannel *channel
) {
  if(
    channel->output_enabled == 1
    && channel->beats < channel->multiplier
    && channel->samples == 0
  ) {
    headwater_state_channel_fire(channel);
  } else {
    channel->output = 0;
  }
}

void headwater_state_channel_reset(
  HeadwaterStateChannel *channel
) {
  channel->beats = 0;
  channel->samples = 0;
  channel->no_remainder_priority = channel->no_remainder_frequency;
  channel->remainder_priority = channel->remainder_frequency;
}

void headwater_state_update_outputs(HeadwaterState *state) {
  headwater_state_channel_update_output(&state->bpm_channel);

  if(state->bpm_channel.output == 1) {
    headwater_state_channel_reset(&state->multiplier_a_channel);
    headwater_state_channel_reset(&state->multiplier_b_channel);
  }

  headwater_state_channel_update_output(&state->multiplier_a_channel);
  headwater_state_channel_update_output(&state->multiplier_b_channel);
}

void headwater_state_channel_increment_counts(
  HeadwaterStateChannel *channel
) {
  if(channel->samples < UINT16_MAX)
    channel->samples++;

  if(channel->beats < channel->multiplier) {
    if(channel->samples_per_beat_with_remainder <= channel->samples) {
      channel->samples = 0;
    }
  }
}

void headwater_state_increment_counts(HeadwaterState *state) {
  headwater_state_channel_increment_counts(&state->bpm_channel);
  headwater_state_channel_increment_counts(&state->multiplier_a_channel);
  headwater_state_channel_increment_counts(&state->multiplier_b_channel);

  // timeout & reset
  if(state->samples_since_reset_count < UINT16_MAX) {
    state->samples_since_reset_count++;
  } else {
    state->reset_count = 0;
    state->samples_since_reset_count = 0;
  }

  // TODO channels should have a loop param to not increment beats
  // then the modes that use internal clock for bpm set bpm channel
  // to loop

  // loop based on internal clock for internal and tap modes
  if(
    state->mode == HEADWATER_STATE_MODE_INTERNAL
    || state->mode == HEADWATER_STATE_MODE_TAP
  ) {
    state->bpm_channel.beats = 0;
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
