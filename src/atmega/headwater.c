#include "stdint.h"
#include "../spi.h"
#include "spi.h"
#include "../clock.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>

#define INPUT_PORT PORTD

#define STOP_PIN PORTD0
#define RESET_PIN PORTD1
#define PLAY_PIN PORTD2

#define OUTPUT_PORT PORTC
#define OUTPUT_DDR DDRC

#define BPM_PIN PORTC0
#define MULTIPLIED_PIN PORTC1

#define UI_HEADER_PLAY 0
#define UI_HEADER_STOP 1
#define UI_HEADER_RESET 2
#define UI_HEADER_UPDATE 3

#define UI_TRANSFER_STATE_HEADER 0
#define UI_TRANSFER_STATE_RESET_HIGH 1
#define UI_TRANSFER_STATE_RESET_LOW 2
#define UI_TRANSFER_STATE_BPM_HIGH 3
#define UI_TRANSFER_STATE_BPM_LOW 4
#define UI_TRANSFER_STATE_MULTA_HIGH 5
#define UI_TRANSFER_STATE_MULTA_LOW 6

static volatile uint8_t ui_transfer_state = UI_TRANSFER_STATE_HEADER;
static volatile uint16_t ui_transfer_reset_cache = 0;
static volatile uint16_t ui_transfer_bpm_cache = 0;
static volatile uint16_t ui_transfer_multa_cache = 0;

ClockState clock_state;

/* volatile uint16_t samples_since_last_reset = 0; // TODO in clock_state */
/* volatile uint16_t samples_since_last_reset = 43605; // TODO in clock_state */

void atmega_update_output(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT &= ~(1 << BPM_PIN);
  } else {
    OUTPUT_PORT |= (1 << BPM_PIN);
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
  OUTPUT_DDR |= (1 << BPM_PIN) | (1 << MULTIPLIED_PIN);

  // Enable play, stop, reset input
  INPUT_PORT |= (1 << PLAY_PIN) | (1 << RESET_PIN) | (1 << STOP_PIN);

  // Pin change interrupt control register
  PCICR = (1 << PCIE2);

  // Configure pin change masks
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

  spi_slave_init();
  sei();

  clock_state = create_clock_state();
  start_clock(&clock_state);

  while(1) {}
}

ISR(TIMER1_COMPA_vect) {
  cycle_clock(
    &clock_state,
    &atmega_update_output,
    &atmega_update_multiplied
  );
}

ISR(SPI_STC_vect) {
  if(ui_transfer_state == UI_TRANSFER_STATE_HEADER) {
    uint8_t header = SPDR;
    if(header & (1 << UI_HEADER_PLAY)) {
      start_clock(&clock_state);
    } else if(header & (1 << UI_HEADER_STOP)) {
      stop_clock(&clock_state);
    } else if(header & (1 << UI_HEADER_RESET)) {
      spi_queue_transfer(spi_get_high_byte(ui_transfer_reset_cache));
      ui_transfer_state = UI_TRANSFER_STATE_RESET_HIGH;
    } else if(header & (1 << UI_HEADER_UPDATE)) {
      ui_transfer_bpm_cache = 0;
      ui_transfer_multa_cache = 0;
      ui_transfer_state = UI_TRANSFER_STATE_BPM_HIGH;
    }
  } else if(ui_transfer_state == UI_TRANSFER_STATE_RESET_HIGH) {
    spi_queue_transfer(spi_get_low_byte(ui_transfer_reset_cache));
    ui_transfer_state = UI_TRANSFER_STATE_RESET_LOW;
  } else if(ui_transfer_state == UI_TRANSFER_STATE_RESET_LOW) {
    ui_transfer_state = UI_TRANSFER_STATE_HEADER;
  } else if(ui_transfer_state == UI_TRANSFER_STATE_BPM_HIGH) {
    ui_transfer_bpm_cache = (SPDR << 8);
    ui_transfer_state = UI_TRANSFER_STATE_BPM_LOW;
  } else if(ui_transfer_state == UI_TRANSFER_STATE_BPM_LOW) {
    ui_transfer_bpm_cache |= SPDR;
    ui_transfer_state = UI_TRANSFER_STATE_MULTA_HIGH;
  } else if(ui_transfer_state == UI_TRANSFER_STATE_MULTA_HIGH) {
    ui_transfer_multa_cache = (SPDR << 8);
    ui_transfer_state = UI_TRANSFER_STATE_MULTA_LOW;
  } else if(ui_transfer_state == UI_TRANSFER_STATE_MULTA_LOW) {
    ui_transfer_multa_cache |= SPDR;
    ui_transfer_state = UI_TRANSFER_STATE_HEADER;

    update_clock_config(
      &clock_state,
      ui_transfer_bpm_cache,
      ui_transfer_multa_cache
    );
  }
}

// TODO make work, debounce
ISR(PCINT2_vect) {
  if(!(PIND & (1 << STOP_PIN))) {
    stop_clock(&clock_state);
  }

  if(!(PIND & (1 << RESET_PIN))) {
    ui_transfer_reset_cache = clock_state.samples_since_reset_count;
    reset_clock(&clock_state);
  }

  if(!(PIND & (1 << PLAY_PIN))) {
    start_clock(&clock_state);
  }
}
