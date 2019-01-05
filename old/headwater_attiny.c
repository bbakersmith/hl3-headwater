#include "stdint.h"
#include "headwater.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// include avr lib


static uint8_t OUTPUT_PIN = PB0;
static uint8_t MULTIPLIED_PIN = PB1;


// should be volatile?
ClockState clock_state;


void attiny_update_output(uint8_t enabled) {
  if(enabled == 0) {
    PORTB &= ~(1 << OUTPUT_PIN);
  } else {
    PORTB |= (1 << OUTPUT_PIN);
  }
}


void attiny_update_multiplied(uint8_t enabled) {
  if(enabled == 0) {
    PORTB &= ~(1 << MULTIPLIED_PIN);
  } else {
    PORTB |= (1 << MULTIPLIED_PIN);
  }
}


void main(void) {
  // enable pins
  DDRB = (1 << OUTPUT_PIN) | (1 << MULTIPLIED_PIN);

  // configure timer and clock state
  OCR1C = 90; // reset count
  TCCR1 |= (1 << PWM1A); // enable pwm
  TCCR1 |= (0 << CS13) | (1 << CS12) | (0 << CS11) | (1 << CS10); // prescale

  // enable interrupts
  TIMSK |= (1 << TOIE1);
  sei();

  clock_state = create_clock_state();

  // TODO remove
  update_clock_config(&clock_state, 10, 3);
  start_clock(&clock_state);

  while(1) {}
}


ISR(TIMER1_OVF_vect) {
  cycle_clock(
    &clock_state,
    &attiny_update_output,
    &attiny_update_multiplied
  );
}

// encoder interrupts


// stop / start interrupts


