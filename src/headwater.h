#include "stdint.h"


struct ClockState {
  int16_t tbpm;
  int8_t multiplier;
  uint16_t samples_per_output;
  uint16_t samples_per_output_multiplied;
  uint16_t sample_count;
  uint16_t multiplier_count;
};


int8_t rotary_encoder_modifier(int8_t b_value);
int8_t tbpm_encoder_modifier(int8_t b_value, int8_t tenths_button);
int16_t modify_tbpm(int16_t tbpm, int8_t b_value, int8_t tenths_button);
int8_t modify_multiplier(int8_t multiplier, int8_t b_value);
uint16_t samples_per_output(uint16_t tbpm);
uint16_t samples_per_output_multiplied(uint16_t tbpm, uint8_t multiplier);
struct ClockState create_clock_state();
void update_clock_config(struct ClockState *clock_state, int16_t tbpm, int8_t multiplier);
