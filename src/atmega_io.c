#include "atmega_io.h"

void atmega_io_bpm_output(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT |= (1 << BPM_PIN);
  } else {
    OUTPUT_PORT &= ~(1 << BPM_PIN);
  }
}

void atmega_io_multiplier_a_output(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT |= (1 << MULTIPLIER_A_PIN);
  } else {
    OUTPUT_PORT &= ~(1 << MULTIPLIER_A_PIN);
  }
}

void atmega_io_multiplier_b_output(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT |= (1 << MULTIPLIER_B_PIN);
  } else {
    OUTPUT_PORT &= ~(1 << MULTIPLIER_B_PIN);
  }
}

uint8_t atmega_io_sample_inputs(uint8_t count) {
  // snapshot inputs on falling edge of shift
  // UI_INPUT_PIN (Q) immediately set to H input
  bytes_unset_bit(&UI_PORT, UI_SHIFT_PIN);
  bytes_set_bit(&UI_PORT, UI_SHIFT_PIN);

  uint8_t flags = 0;
  for(uint8_t i = 0; i < count; i++) {
    flags |= (bytes_check_bit(UI_PIN, UI_INPUT_PIN) << i);

    // don't advance the clock after the last value
    if(i != (count - 1)) {
      bytes_set_bit(&UI_PORT, UI_CLK_PIN);
      bytes_unset_bit(&UI_PORT, UI_CLK_PIN);
    }
  }

  return flags;
}

void atmega_io_init(void) {
  // disable outputs
  atmega_io_bpm_output(0);
  atmega_io_multiplier_a_output(0);
  atmega_io_multiplier_b_output(0);

  // enable output pins
  OUTPUT_DDR |=
    (1 << BPM_PIN)
    | (1 << MULTIPLIER_A_PIN)
    | (1 << MULTIPLIER_B_PIN);

  // enable ui input
  UI_PORT |= (1 << UI_INPUT_PIN);

  // enable ui output
  UI_DDR |= (1<< UI_SHIFT_PIN) | (1 << UI_CLK_PIN);
}
