#include "stdint.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#include "atmega_spi.h"
#include "headwater_state.h"
#include "spi.h"

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

enum SPI_HEADER {
  SPI_HEADER_PLAY,
  SPI_HEADER_STOP,
  SPI_HEADER_RESET,
  SPI_HEADER_UPDATE
};

enum SPI_STATE {
  SPI_STATE_HEADER,
  SPI_STATE_RESET_HIGH,
  SPI_STATE_RESET_LOW,
  SPI_STATE_BPM_HIGH,
  SPI_STATE_BPM_LOW,
  SPI_STATE_MULTA_HIGH,
  SPI_STATE_MULTA_LOW
};

static volatile uint8_t spi_state = SPI_STATE_HEADER;
static volatile uint16_t spi_reset_cache = 0;
static volatile uint16_t spi_bpm_cache = 0;
static volatile uint16_t spi_multa_cache = 0;

HeadwaterState headwater_state;

void atmega_headwater_output_fn(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT |= (1 << BPM_PIN);
  } else {
    OUTPUT_PORT &= ~(1 << BPM_PIN);
  }
}

void atmega_headwater_multiplied_fn(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT |= (1 << MULTIPLIED_PIN);
  } else {
    OUTPUT_PORT &= ~(1 << MULTIPLIED_PIN);
  }
}

void main(void) {
  // enable output pins
  OUTPUT_DDR |= (1 << BPM_PIN) | (1 << MULTIPLIED_PIN);

  // enable play, stop, reset input
  INPUT_PORT |= (1 << PLAY_PIN) | (1 << RESET_PIN) | (1 << STOP_PIN);

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

  headwater_state = create_headwater_state();
  headwater_state_start(&headwater_state);

  while(1) {}
}

ISR(TIMER1_COMPA_vect) {
  headwater_state_cycle(
    &headwater_state,
    &atmega_headwater_output_fn,
    &atmega_headwater_multiplied_fn
  );
}

ISR(SPI_STC_vect) {
  if(spi_state == SPI_STATE_HEADER) {
    uint8_t header = SPDR;
    if(header & (1 << SPI_HEADER_PLAY)) {
      headwater_state_start(&headwater_state);
    } else if(header & (1 << SPI_HEADER_STOP)) {
      headwater_state_stop(&headwater_state);
    } else if(header & (1 << SPI_HEADER_RESET)) {
      atmega_spi_queue_transfer(spi_get_high_byte(spi_reset_cache));
      spi_state = SPI_STATE_RESET_HIGH;
    } else if(header & (1 << SPI_HEADER_UPDATE)) {
      spi_bpm_cache = 0;
      spi_multa_cache = 0;
      spi_state = SPI_STATE_BPM_HIGH;
    }
  } else if(spi_state == SPI_STATE_RESET_HIGH) {
    atmega_spi_queue_transfer(spi_get_low_byte(spi_reset_cache));
    spi_state = SPI_STATE_RESET_LOW;
  } else if(spi_state == SPI_STATE_RESET_LOW) {
    spi_state = SPI_STATE_HEADER;
  } else if(spi_state == SPI_STATE_BPM_HIGH) {
    spi_bpm_cache = (SPDR << 8);
    spi_state = SPI_STATE_BPM_LOW;
  } else if(spi_state == SPI_STATE_BPM_LOW) {
    spi_bpm_cache |= SPDR;
    spi_state = SPI_STATE_MULTA_HIGH;
  } else if(spi_state == SPI_STATE_MULTA_HIGH) {
    spi_multa_cache = (SPDR << 8);
    spi_state = SPI_STATE_MULTA_LOW;
  } else if(spi_state == SPI_STATE_MULTA_LOW) {
    spi_multa_cache |= SPDR;
    spi_state = SPI_STATE_HEADER;

    headwater_state_update(
      &headwater_state,
      spi_bpm_cache,
      spi_multa_cache
    );
  }
}

// TODO make work, debounce
ISR(PCINT2_vect) {
  if(!(PIND & (1 << STOP_PIN))) {
    headwater_state_stop(&headwater_state);
  }

  if(!(PIND & (1 << RESET_PIN))) {
    spi_reset_cache = headwater_state.samples_since_reset_count;
    headwater_state_reset(&headwater_state);
  }

  if(!(PIND & (1 << PLAY_PIN))) {
    headwater_state_start(&headwater_state);
  }
}
