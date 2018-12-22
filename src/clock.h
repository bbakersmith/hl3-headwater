#include "stdint.h"

typedef struct ClockState {
  int8_t multiplier;
  uint8_t running;
  uint16_t reset;
  uint16_t samples_since_reset_count;
  uint16_t samples_per_output;
  uint16_t samples_per_multiplied;
  uint16_t sample_count;
  uint16_t multiplied_sample_count;
  uint16_t multiplied_count;
  uint8_t output;
  uint8_t multiplied;
} ClockState;

typedef void (UpdateOutputFn)(uint8_t enabled);

struct ClockState create_clock_state();
void update_clock_config(struct ClockState *clock_state, int16_t tbpm, int8_t multiplier);
void stop_clock(struct ClockState *clock_state);
void reset_clock(struct ClockState *clock_state);
void start_clock(struct ClockState *clock_state);
void cycle_clock(struct ClockState *clock_state, UpdateOutputFn output_fn, UpdateOutputFn multiplied_fn);
