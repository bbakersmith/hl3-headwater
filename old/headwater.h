#include "stdint.h"


typedef struct ClockState {
  int8_t ext_mode;
  int16_t tbpm;
  int8_t multiplier;
  uint8_t running;
  uint16_t samples_per_output;
  uint16_t samples_per_multiplied;
  uint16_t sample_count;
  uint16_t multiplied_sample_count;
  uint16_t multiplied_count;
  uint8_t output;
  uint8_t multiplied;
} ClockState;


typedef void (UpdateOutputFn)(uint8_t enabled);


int8_t rotary_encoder_modifier(int8_t b_value);
int8_t tbpm_encoder_modifier(int8_t b_value, int8_t tenths_button);
int16_t modify_tbpm(int16_t tbpm, int8_t b_value, int8_t tenths_button);
int8_t modify_multiplier(int8_t multiplier, int8_t b_value);
uint16_t samples_per_output(int16_t tbpm);
uint16_t samples_per_multiplied(int16_t tbpm, int8_t multiplier);
struct ClockState create_clock_state();
void update_clock_config(struct ClockState *clock_state, int16_t tbpm, int8_t multiplier);
void increment_clock(struct ClockState *clock_state);
void update_clock_outputs(struct ClockState *clock_state, UpdateOutputFn output_fn, UpdateOutputFn multiplied_fn);
void stop_clock(struct ClockState *clock_state);
void start_clock(struct ClockState *clock_state);
void cycle_clock(struct ClockState *clock_state, UpdateOutputFn output_fn, UpdateOutputFn multiplied_fn);