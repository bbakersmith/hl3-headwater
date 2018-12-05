#include "stdint.h"
#include "headwater.h"
#include <avr/interrupt.h>
#include <avr/io.h>


#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>


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


// atmega SPI functionality


// mosi output
// miso input (implied by master mode)
// sck output
// ss output


#define MOSI PORTB3
#define SCK PORTB5
#define SS PORTB2
#define ENC1A PORTD0
#define ENC1B PORTD1
#define ENC1SW PORTD2


void spi_master_init(void) {
  // set CS high
  PORTB |= (1 << SS);
  /* Set MOSI, SCK, and SS (PORTB2) output, all others input */
  DDRB = (1 << MOSI) | (1 << SCK) | (1 << SS);
  /* Enable SPI, Master, set clock rate fck/16 */
  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR1) | (1 << SPR0);
}


uint8_t spi_master_transmit(uint8_t data) {
  /* Start transmission */
  SPDR = data;
  /* Wait for transmission complete */
  while(!(SPSR & (1 << SPIF)));
  /* Return data from MISO? */
  return(SPDR);
}


static uint8_t MAX7221_DIGIT_0 = 1;
static uint8_t MAX7221_DIGIT_1 = 2;
static uint8_t MAX7221_DIGIT_2 = 3;
static uint8_t MAX7221_DIGIT_3 = 4;
static uint8_t MAX7221_DIGIT_4 = 5;
static uint8_t MAX7221_DIGIT_5 = 6;
static uint8_t MAX7221_DIGIT_6 = 7;
static uint8_t MAX7221_DIGIT_7 = 8;
static uint8_t MAX7221_DECODE_MODE = 9;
static uint8_t MAX7221_INTENSITY = 10;
static uint8_t MAX7221_SCAN_LIMIT = 11;
static uint8_t MAX7221_SHUTDOWN = 12;
static uint8_t MAX7221_DISPLAY_TEST = 16;

static uint8_t MAX7221_DIGIT_DECIMAL_BIT = 7;
static uint8_t MAX7221_DIGIT_VALUE_DASH = 10;
static uint8_t MAX7221_DIGIT_VALUE_OFF = 16;

static uint8_t MAX7221_INTENSITY_MIN = 0;
static uint8_t MAX7221_INTENSITY_MAX = 16;

static uint8_t MAX7221_SHUTDOWN_DISABLE = 1;
static uint8_t MAX7221_SHUTDOWN_ENABLE = 0;

static uint8_t MAX7221_DISPLAY_TEST_DISABLE = 0;
static uint8_t MAX7221_DISPLAY_TEST_ENABLE = 1;


void max7221_write(uint8_t address, uint8_t data) {
  // set CS low
  PORTB &= ~(1 << SS);

  spi_master_transmit(address);
  spi_master_transmit(data);

  // set CS high
  PORTB |= (1 << SS);
}


void max7221_start(void) {
  max7221_write(MAX7221_SHUTDOWN, 1);
}


void max7221_stop(void) {
  max7221_write(MAX7221_SHUTDOWN, 0);
}


void max7221_intensity(uint8_t level) {
  // 0 - 16
  max7221_write(MAX7221_INTENSITY, level);
}


void max7221_code_b_decode(uint8_t digits) {
  max7221_write(MAX7221_DECODE_MODE, digits);
}


void max7221_limit_digits(uint8_t num_digits) {
  // 1 - 8
  max7221_write(MAX7221_SCAN_LIMIT, num_digits - 1);
}


void max7221_enable_display_test(void) {
  max7221_write(MAX7221_DISPLAY_TEST, 1);
}


void max7221_disable_display_test(void) {
  max7221_write(MAX7221_DISPLAY_TEST, 0);
}


uint8_t max7221_digit_with_decimal(uint8_t digit) {
  return digit |= (1 << MAX7221_DIGIT_DECIMAL_BIT);
}


void max7221_write_blank(uint8_t digit) {
  max7221_write(digit, MAX7221_DIGIT_VALUE_OFF);
}


void max7221_write_dash(uint8_t digit) {
  max7221_write(digit, MAX7221_DIGIT_VALUE_DASH);
}

void max7221_write_4digit_number(uint16_t number) {
  uint8_t index = 4;
  while(0 < index) {
    uint8_t digit = 0;
    if(0 < number) {
      digit = number % 10;
      number /= 10;
    }
    max7221_write(index, digit);
    index -= 1;
  }
}

void max7221_initialize(uint8_t intensity, uint8_t num_digits) {
  max7221_code_b_decode(0xFF);
  max7221_intensity(intensity);
  max7221_limit_digits(num_digits);
  max7221_start();
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

void main(void) {
  // enable pins
  /* INPUT_DDR = (1 << TBPM_ENCODER_A) | (1 << TBPM_ENCODER_B); */
  OUTPUT_DDR = (1 << OUTPUT_PIN) | (1 << MULTIPLIED_PIN) | (1 << DEBUG_PIN);

  /* Enable rotary encoder inputs */
  PORTD |= (1 << ENC1A) | (1 << ENC1B) | (1 << ENC1SW);

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

  _delay_ms(1000);
  spi_master_init();
  max7221_initialize(0, 4);

  clock_state = create_clock_state();
  update_clock_config(&clock_state, 1200, 2);
  start_clock(&clock_state);

  uint8_t enc1_enabled = 0;
  uint8_t enc1sw_enabled = 0;

  /*
  uint32_t x = 200000;
  uint16_t y = 28;
  total = x / y;
  uint16_t number = 3001;
  uint16_t number = 1089;
  */

  while(1) {
    /* handle rotary encoder input */
    if(!(PIND & (1 << ENC1A))) {
      if(enc1_enabled == 0) {
        uint16_t new_tbpm;
        enc1_enabled = 1;
        if(!(PIND & (1 << ENC1B))) {
          new_tbpm = clock_state.tbpm + 10;
        } else {
          new_tbpm = clock_state.tbpm - 10;
        }
        update_clock_config(&clock_state, new_tbpm, clock_state.multiplier);
      }
    } else {
      enc1_enabled = 0;
    }

    /* handle rotary encoder pushbutton input */
    if(!(PIND & (1 << ENC1SW))) {
      if(enc1sw_enabled == 0) {
        enc1sw_enabled = 1;
        update_clock_config(&clock_state, 1200, clock_state.multiplier);
      }
    } else {
      enc1sw_enabled = 0;
    }

    max7221_write_4digit_number(clock_state.tbpm);

    /* total += 1; */
    /* max7221_write_4digit_number(total); */
    /* _delay_ms(1000); */
    /* debug_toggle(); */

    /* _delay_ms(1000); */
    /* max7221_intensity(0); */
    /* _delay_ms(1000); */
    /* max7221_intensity(5); */
  }
}


ISR(TIMER1_COMPA_vect) {
  // debug_toggle();

  // debug pin enable
  // OUTPUT_PORT |= (1 << DEBUG_PIN);

  cycle_clock(
    &clock_state,
    &atmega_update_output,
    &atmega_update_multiplied
  );

  // debug pin disable
  // OUTPUT_PORT &= ~(1 << DEBUG_PIN);
}


/*
ISR(PCINT1_vect) {
  // change in input pins

}
*/


// encoder interrupts


// stop / start interrupts


