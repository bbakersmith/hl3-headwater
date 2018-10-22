#include "stdint.h"
#include "headwater.h"


static uint16_t MIN_BPM = 10;
static uint16_t MAX_BPM = 3000;


static uint8_t MIN_MULTIPLIER = 1;
static uint8_t MAX_MULTIPLIER = 99;


static uint8_t SECONDS_IN_MINUTE = 60;
static uint16_t SYSTEM_CLOCK_HZ = 1000;


static int16_t CLOCK_STATE_DEFAULT_TBPM = 1200;
static int8_t CLOCK_STATE_DEFAULT_MULTIPLIER = 1;
static int8_t CLOCK_STATE_DEFAULT_PLAY = 0;
static uint16_t CLOCK_STATE_DEFAULT_SAMPLES_PER_OUTPUT = 500;
static uint16_t CLOCK_STATE_DEFAULT_SAMPLES_PER_MULTIPLIED = 500;
static uint16_t CLOCK_STATE_DEFAULT_SAMPLE_COUNT = 0;
static uint16_t CLOCK_STATE_DEFAULT_MULTIPLIED_COUNT = 0;


int8_t rotary_encoder_modifier(int8_t b_value) {
  if(b_value) {
    return 1;
  } else {
    return -1;
  }
}


int8_t tbpm_encoder_modifier(int8_t b_value, int8_t tenths_button) {
  int8_t modifier = rotary_encoder_modifier(b_value);

  if(tenths_button == 0) {
    modifier *= 10;
  }

  return modifier;
}


int16_t modify_tbpm(int16_t tbpm, int8_t b_value, int8_t tenths_button) {
  int8_t modifier =tbpm_encoder_modifier(b_value, tenths_button);
  int16_t new_tbpm = tbpm + modifier;

  if(new_tbpm < MIN_BPM) {
    new_tbpm = MIN_BPM;
  } else if(MAX_BPM < new_tbpm) {
    new_tbpm = MAX_BPM;
  }

  return new_tbpm;
}


int8_t modify_multiplier(int8_t multiplier, int8_t b_value) {
  int8_t modifier = rotary_encoder_modifier(b_value);
  int8_t new_multiplier = multiplier + modifier;

  if(new_multiplier < MIN_MULTIPLIER) {
    new_multiplier = MIN_MULTIPLIER;
  } else if(MAX_MULTIPLIER < new_multiplier) {
    new_multiplier = MAX_MULTIPLIER;
  }

  return new_multiplier;
}


// TODO: update LED functions


uint16_t samples_per_output(uint16_t tbpm) {
  return (SYSTEM_CLOCK_HZ * SECONDS_IN_MINUTE * 10) / tbpm;
}


uint16_t samples_per_multiplied(uint16_t tbpm, uint8_t multiplier) {
  return samples_per_output(tbpm) / multiplier;
}


ClockState create_clock_state() {
  ClockState clock_state;
  clock_state.tbpm = CLOCK_STATE_DEFAULT_TBPM;
  clock_state.multiplier = CLOCK_STATE_DEFAULT_MULTIPLIER;
  clock_state.running = CLOCK_STATE_DEFAULT_PLAY;
  clock_state.samples_per_output = CLOCK_STATE_DEFAULT_SAMPLES_PER_OUTPUT;
  clock_state.samples_per_multiplied = CLOCK_STATE_DEFAULT_SAMPLES_PER_MULTIPLIED;
  clock_state.sample_count = CLOCK_STATE_DEFAULT_SAMPLE_COUNT;
  clock_state.multiplied_count = CLOCK_STATE_DEFAULT_MULTIPLIED_COUNT;
  return clock_state;
}


void update_clock_config(ClockState *clock_state, int16_t tbpm, int8_t multiplier) {
  clock_state->tbpm = tbpm;
  clock_state->multiplier = multiplier;
  clock_state->samples_per_output = samples_per_output(tbpm);
  clock_state->samples_per_multiplied = samples_per_multiplied(tbpm, multiplier);
}


void stop_clock(ClockState *clock_state) {
  clock_state->running = 0;
}


void start_clock(ClockState *clock_state) {
  clock_state->running = 1;
  clock_state->sample_count = 0;
  clock_state->multiplied_count = 0;
}


void increment_clock(ClockState *clock_state) {
  clock_state->sample_count += 1;
  if(clock_state->samples_per_output <= clock_state->sample_count) {
    clock_state->sample_count = 0;
    clock_state->multiplied_count = 0;
  }
}


void update_clock_outputs(ClockState *clock_state, UpdateOutputFn output_fn, UpdateOutputFn multiplied_fn) {
  uint8_t remainder;
  if(clock_state->sample_count == 0) {
    output_fn(1);
    multiplied_fn(1);
    clock_state->multiplied_count += 1;
  } else {
    output_fn(0);
    remainder = clock_state->sample_count % clock_state->samples_per_multiplied;
    if(remainder == 0 && clock_state->multiplied_count < clock_state->multiplier) {
      multiplied_fn(1);
      clock_state->multiplied_count += 1;
    } else {
      multiplied_fn(0);
    }
  }
}


void cycle_clock(ClockState *clock_state, UpdateOutputFn output_fn, UpdateOutputFn multiplied_fn) {
  if(clock_state->running == 1) {
    update_clock_outputs(clock_state, output_fn, multiplied_fn);
    increment_clock(clock_state);
  } else {
    // do nothing
  }
}
