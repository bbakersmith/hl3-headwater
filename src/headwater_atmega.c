#include "stdint.h"
#include "headwater.h"
#include <avr/interrupt.h>
#include <avr/io.h>


#define INPUT_PORT PORTC
#define INPUT_DDR DDRC


#define OUTPUT_PORT PORTB
#define OUTPUT_DDR DDRB


static uint8_t TBPM_ENCODER_A = PORTC0;
static uint8_t TBPM_ENCODER_B = PORTC1;


static uint8_t OUTPUT_PIN = PORTB0;
static uint8_t MULTIPLIED_PIN = PORTB1;
static uint8_t DEBUG_PIN = PORTB2;


// should be volatile?
ClockState clock_state;


void atmega_update_output(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT &= ~(1 << OUTPUT_PIN);
  } else {
    OUTPUT_PORT |= (1 << OUTPUT_PIN);
  }
}


void atmega_update_multiplied(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT &= ~(1 << MULTIPLIED_PIN);
  } else {
    OUTPUT_PORT |= (1 << MULTIPLIED_PIN);
  }
}


void main(void) {
  // enable pins
  INPUT_DDR = (1 << TBPM_ENCODER_A) | (1 << TBPM_ENCODER_B);
  OUTPUT_DDR = (1 << OUTPUT_PIN) | (1 << MULTIPLIED_PIN) | (1 << DEBUG_PIN);

  // timer clear timer on compare (ctc) mode
  TCCR1A |= (0 << WGM11) | (0 << WGM10);
  TCCR1B |= (0 << WGM13) | (1 << WGM12);

  // timer prescale
  TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);

  // timer reset value
  OCR1A = 2000;

  // enable interrupts
  TIMSK1 |= (1 << OCIE1A);
  PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);
  sei();

  clock_state = create_clock_state();

  // TODO remove
  update_clock_config(&clock_state, 1200, 50);
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

  // debug pin enable
  OUTPUT_PORT |= (1 << DEBUG_PIN);

  cycle_clock(
    &clock_state,
    &atmega_update_output,
    &atmega_update_multiplied
  );

  // debug pin disable
  OUTPUT_PORT &= ~(1 << DEBUG_PIN);
}


ISR(PCINT1_vect) {
  // change in input pins

}

// encoder interrupts


// stop / start interrupts


