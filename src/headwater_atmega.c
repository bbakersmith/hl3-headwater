#include "stdint.h"
#include "headwater.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// include avr lib


static uint8_t OUTPUT_PIN = PORTB0;
static uint8_t MULTIPLIED_PIN = PORTB1;


// should be volatile?
ClockState clock_state;


void atmega_update_output(uint8_t enabled) {
  if(enabled == 0) {
    PORTB &= ~(1 << OUTPUT_PIN);
  } else {
    PORTB |= (1 << OUTPUT_PIN);
  }
}


void atmega_update_multiplied(uint8_t enabled) {
  if(enabled == 0) {
    PORTB &= ~(1 << MULTIPLIED_PIN);
  } else {
    PORTB |= (1 << MULTIPLIED_PIN);
  }
}


void main(void) {
  // enable pins
  DDRB = (1 << OUTPUT_PIN) | (1 << MULTIPLIED_PIN);

  // timer clear timer on compare (ctc) mode
  TCCR1A |= (0 << WGM11) | (0 << WGM10);
  TCCR1B |= (0 << WGM13) | (1 << WGM12);

  // timer prescale
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);

  // timer reset value
  OCR1A = 1500;

  // enable interrupts
  TIMSK1 |= (1 << OCIE1A);
  sei();

  clock_state = create_clock_state();

  // TODO remove
  update_clock_config(&clock_state, 600, 3);
  start_clock(&clock_state);

  while(1) {}
}


uint8_t debug_toggle_state = 0;
void debug_toggle(void) {
  if(debug_toggle_state) {
    atmega_update_output(0);
    atmega_update_multiplied(1);
    debug_toggle_state = 0;
  } else {
    atmega_update_output(1);
    atmega_update_multiplied(0);
    debug_toggle_state = 1;
  }
}


ISR(TIMER1_COMPA_vect) {
  // debug_toggle();

  cycle_clock(
    &clock_state,
    &atmega_update_output,
    &atmega_update_multiplied
  );
}

// encoder interrupts


// stop / start interrupts


