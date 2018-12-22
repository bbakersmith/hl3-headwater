#include "stdint.h"
#include "clock.h"

static int8_t CLOCK_RUNNING_STOP = 0;
static int8_t CLOCK_RUNNING_PLAY = 1;

static int8_t CLOCK_STATE_DEFAULT_RUNNING = 0;
static int8_t CLOCK_STATE_DEFAULT_MULTIPLIER = 4;
static uint16_t CLOCK_STATE_DEFAULT_SAMPLES_PER_OUTPUT = 1000;
static uint16_t CLOCK_STATE_DEFAULT_SAMPLES_PER_MULTIPLIED = 250;
static uint16_t CLOCK_STATE_DEFAULT_SAMPLE_COUNT = 0;
static uint16_t CLOCK_STATE_DEFAULT_MULTIPLIED_SAMPLE_COUNT = 0;
static uint16_t CLOCK_STATE_DEFAULT_MULTIPLIED_COUNT = 0;

// TODO take system clock speed? sample rate?
ClockState create_clock_state() {
  ClockState clock_state;
  clock_state.running = CLOCK_STATE_DEFAULT_RUNNING;
  clock_state.multiplier = CLOCK_STATE_DEFAULT_MULTIPLIER;
  clock_state.reset = 0;
  clock_state.samples_since_reset_count = 0;
  clock_state.samples_per_output = CLOCK_STATE_DEFAULT_SAMPLES_PER_OUTPUT;
  clock_state.samples_per_multiplied = CLOCK_STATE_DEFAULT_SAMPLES_PER_MULTIPLIED;
  clock_state.sample_count = CLOCK_STATE_DEFAULT_SAMPLE_COUNT;
  clock_state.multiplied_sample_count = CLOCK_STATE_DEFAULT_MULTIPLIED_SAMPLE_COUNT;
  clock_state.multiplied_count = CLOCK_STATE_DEFAULT_MULTIPLIED_COUNT;
  return clock_state;
}

void update_clock_config(ClockState *clock_state, int16_t tbpm, int8_t multiplier) {
  clock_state->multiplier = multiplier;
  clock_state->samples_per_output = tbpm;
  clock_state->samples_per_multiplied = tbpm / multiplier;
}

void stop_clock(ClockState *clock_state) {
  clock_state->running = CLOCK_RUNNING_STOP;
}

void reset_clock(ClockState *clock_state) {
  clock_state->sample_count = 0;
  clock_state->multiplied_sample_count = 0;
  clock_state->multiplied_count = 0;
  clock_state->samples_since_reset_count = 0;
}

void start_clock(ClockState *clock_state) {
  clock_state->sample_count = 0;
  clock_state->multiplied_sample_count = 0;
  clock_state->multiplied_count = 0;
  clock_state->running = CLOCK_RUNNING_PLAY;
}

void increment_clock(ClockState *clock_state) {
  clock_state->sample_count += 1;
  clock_state->multiplied_sample_count += 1;
  // TODO prevent overflow, timeout reset
  clock_state->samples_since_reset_count += 1;
  // TODO move this into update clock outputs to reduce branching?
  if(clock_state->samples_per_output <= clock_state->sample_count) {
    clock_state->sample_count = 0;
    clock_state->multiplied_sample_count = 0;
    clock_state->multiplied_count = 0;
  }
}

void update_clock_outputs(ClockState *clock_state, UpdateOutputFn output_fn, UpdateOutputFn multiplied_fn) {
  if(clock_state->sample_count == 0) {
    output_fn(1);
    multiplied_fn(1);
    clock_state->multiplied_count += 1;
  } else {
    output_fn(0);
    if(clock_state->multiplied_count < clock_state->multiplier &&
        clock_state->samples_per_multiplied <= clock_state->multiplied_sample_count) {

      clock_state->multiplied_count += 1;
      clock_state->multiplied_sample_count = 0;
      multiplied_fn(1);
    } else {
      multiplied_fn(0);
    }
  }
}

void cycle_clock(ClockState *clock_state, UpdateOutputFn output_fn, UpdateOutputFn multiplied_fn) {
  // TODO handle this at the interrupt level?
  // ex. play button enables TIMER1_COMPA interrupt, stop disables it
  if(clock_state->running == CLOCK_RUNNING_PLAY) {
    update_clock_outputs(clock_state, output_fn, multiplied_fn);
    increment_clock(clock_state);
  }
}
