#include "stdint.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#include "atmega_spi.h"
#include "headwater_api.h"
#include "headwater_state.h"

#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>

#define INPUT_PORT PORTD

#define STOP_PIN PORTD0
#define RESET_PIN PORTD1
#define PLAY_PIN PORTD2

#define CV_PORT PORTC

#define BPM_CV_PIN PORTC3
#define MULTIPLIER_A_CV_PIN PORTC4
#define MULTIPLIER_B_CV_PIN PORTC5

#define OUTPUT_PORT PORTC
#define OUTPUT_DDR DDRC

#define BPM_PIN PORTC0
#define MULTIPLIER_A_PIN PORTC1

API volatile headwater_api;

void atmega_headwater_bpm_output(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT |= (1 << BPM_PIN);
  } else {
    OUTPUT_PORT &= ~(1 << BPM_PIN);
  }
}

void atmega_headwater_multiplier_a_output(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT |= (1 << MULTIPLIER_A_PIN);
  } else {
    OUTPUT_PORT &= ~(1 << MULTIPLIER_A_PIN);
  }
}

void main(void) {
  // enable output pins
  OUTPUT_DDR |= (1 << BPM_PIN) | (1 << MULTIPLIER_A_PIN);

  // enable play, stop, reset input
  INPUT_PORT |= (1 << PLAY_PIN) | (1 << RESET_PIN) | (1 << STOP_PIN);
  CV_PORT |=
    (1 << BPM_CV_PIN)
    | (1 << MULTIPLIER_A_CV_PIN)
    | (1 << MULTIPLIER_B_CV_PIN);

  // pin change interrupt control register
  PCICR = (1 << PCIE2);

  // configure pin change masks
  PCMSK2 = (1 << PLAY_PIN) | (1 << RESET_PIN) | (1 << STOP_PIN);

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

  atmega_spi_slave_init();
  sei();

  atmega_headwater_bpm_output(0);
  atmega_headwater_multiplier_a_output(0);

  HeadwaterState headwater_state = headwater_state_new();
  APIRequest headwater_api_request = api_new_request();

  API headwater_api_ = {
    .state = headwater_state,
    .request = headwater_api_request,
    .payload_preprocessor = &headwater_api_payload_preprocessor,
    .payload_postprocessor = &headwater_api_payload_postprocessor
  };
  headwater_api = headwater_api_;

  // TODO don't start automatically
  headwater_state_play(&headwater_api.state);

  while(1) {
    if(headwater_api.state.change_flags == 0) {
      // TODO cv
    } else {
      headwater_state_change(&headwater_api.state);
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  headwater_state_cycle(&headwater_api.state);
  atmega_headwater_bpm_output(headwater_api.state.bpm_channel.output);
  atmega_headwater_multiplier_a_output(
    headwater_api.state.multiplier_a_channel.output
  );
}

ISR(SPI_STC_vect) {
  SPDR = api_handle_interrupt(&headwater_api, SPDR);
}

// TODO debounce
ISR(PCINT2_vect) {
  if(!(PIND & (1 << PLAY_PIN))) {
    headwater_api.state.change_flags |= (1 << HEADWATER_STATE_CHANGE_PLAY);
  }

  if(!(PIND & (1 << RESET_PIN))) {
    headwater_api.state.change_flags |= (1 << HEADWATER_STATE_CHANGE_RESET);
    /* uint16_t spi_reset_cache = headwater_api.state.samples_since_reset_count; */
  }

  if(!(PIND & (1 << STOP_PIN))) {
    headwater_api.state.change_flags |= (1 << HEADWATER_STATE_CHANGE_STOP);
  }
}
