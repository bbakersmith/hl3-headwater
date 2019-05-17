/**
 * @file headwater_state.h
 */

// TODO doxygen

#ifndef _HEADWATER_STATE_H_
#define _HEADWATER_STATE_H_

#include "stdbool.h"
#include "stdint.h"

#include "midi.h"

#define SECONDS_IN_MINUTE 60
#define HEADWATER_STATE_DEFAULT_BPM 600

/**
 * TODO
 */
typedef enum {
  HEADWATER_STATE_CHANGE_STOP,
  HEADWATER_STATE_CHANGE_PLAY,
  HEADWATER_STATE_CHANGE_MODE,
  HEADWATER_STATE_CHANGE_BPM,
  HEADWATER_STATE_CHANGE_MULTIPLIER_A,
  HEADWATER_STATE_CHANGE_MULTIPLIER_B
} HeadwaterStateChange;

/**
 * TODO
 */
typedef enum {
  HEADWATER_STATE_MODE_INTERNAL,
  HEADWATER_STATE_MODE_TAP,
  HEADWATER_STATE_MODE_EXTERNAL,
  HEADWATER_STATE_MODE_MIDI
} HeadwaterStateMode;

/**
 * TODO
 */
typedef volatile struct HeadwaterStateChannel {
  uint16_t beats;
  uint16_t limit;
  uint16_t no_remainder_frequency;
  uint16_t no_remainder_priority;
  uint8_t multiplier;
  uint8_t output;
  uint16_t remainder_frequency;
  uint16_t remainder_priority;
  uint16_t samples_per_beat;
  uint16_t samples_per_beat_with_remainder;
  uint16_t samples;
} HeadwaterStateChannel;

/**
 * TODO
 */
typedef volatile struct HeadwaterState {
  uint16_t bpm;
  HeadwaterStateChannel bpm_channel;
  uint8_t change_flags;
  HeadwaterStateChannel midi_channel;
  MIDI *midi;
  uint8_t mode;
  HeadwaterStateChannel multiplier_a_channel;
  HeadwaterStateChannel multiplier_b_channel;
  uint8_t output_enabled; // TODO should be boolean
  uint8_t preset;
  uint16_t reset_count;
  uint32_t samples_per_second;
  uint16_t samples_since_reset_count; // TODO used? rename?
} HeadwaterState;

/**
 * TODO should be able to avoid dealing with this in state module
 */
typedef void (HeadwaterOutputFn)(uint8_t enabled);

/**
 * TODO
 */
HeadwaterStateChannel headwater_state_channel_new(uint16_t samples_per_beat);

/**
 * TODO
 */
HeadwaterState headwater_state_new(void);

/**
 * TODO
 */
uint16_t headwater_state_samples_to_bpm(
  uint32_t samples_per_second,
  uint16_t samples_per_beat
);

/**
 * TODO
 */
uint16_t headwater_state_bpm_to_samples(
  uint32_t samples_per_second,
  uint16_t bpm
);

/**
 * TODO
 */
void headwater_state_increment_counts(HeadwaterState *state);

/**
 * TODO
 */
uint16_t headwater_state_apply_modifier(uint16_t value, uint8_t modifier);

/**
 * TODO
 */
void headwater_state_update_samples_per_beat(
  HeadwaterStateChannel *channel,
  uint16_t samples_per_beat
);

/**
 * TODO
 */
void headwater_state_stop(HeadwaterState *state);

/**
 * Handle triggers from play button, external input jack, and
 * MIDI start / continue commands.
 */
void headwater_state_play(HeadwaterState *state);

/**
 * TODO
 */
void headwater_state_external_reset(HeadwaterState *state);

/**
 * TODO
 */
void headwater_state_channel_fire(HeadwaterStateChannel *channel);

/**
 * TODO rename headwater_state_handle_interrupt
 */
void headwater_state_cycle(HeadwaterState *state);

/**
 * TODO
 */
bool headwater_state_has_change_now(uint8_t change_flags);

/**
 * TODO
 */
void headwater_state_handle_change_now(HeadwaterState *state);

/**
 * TODO
 */
bool headwater_state_has_change_after_beat(uint8_t change_flags);

/**
 * TODO
 */
void headwater_state_handle_change_after_beat(HeadwaterState *state);

/**
 * TODO
 */
void headwater_state_handle_midi_read(HeadwaterState *state);

#endif
