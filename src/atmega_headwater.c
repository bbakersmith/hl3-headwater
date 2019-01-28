#define F_CPU 16000000UL  // 16 MHz

#include "stdint.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "atmega_lcd.h"
#include "atmega_spi.h"
#include "bytes.h"
#include "headwater_api.h"
#include "headwater_state.h"
#include "lcd.h"

#define INPUT_PORT PORTD

#define STOP_PIN PORTD0
#define RESET_PIN PORTD1
#define PLAY_PIN PORTD2

#define OUTPUT_PORT PORTC
#define OUTPUT_DDR DDRC

#define BPM_PIN PORTC0
#define MULTIPLIER_A_PIN PORTC1

volatile API headwater_api;
volatile LCD lcd_state;

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

  // pin change interrupt control register
  PCICR = (1 << PCIE2);

  // configure pin change masks
  PCMSK2 = (1 << PLAY_PIN) | (1 << RESET_PIN) | (1 << STOP_PIN);

  // timer clear timer on compare (ctc) mode
  TCCR0A |= (1 << WGM01);
  TCCR1B |= (1 << WGM12);

  // timer prescale
  TCCR0B |= (1 << CS02) | (1 << CS00); // 1/1024
  TCCR1B |= (1 << CS11);

  // timer reset value
  OCR0A = 100;
  OCR1A = 2000;

  // enable interrupts
  TIMSK0 |= (1 << OCIE0A);
  TIMSK1 |= (1 << OCIE1A);
  PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);

  atmega_lcd_init();
  atmega_spi_slave_init();

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

  // wait for LCD
  _delay_ms(500);

  // TODO create lcd state, screens
  LCD lcd_state_ = lcd_new();
  lcd_state = lcd_state_;

  lcd_state.characters[4] = LCD__H;
  lcd_state.characters[5] = LCD__E;
  lcd_state.characters[6] = LCD__L;
  lcd_state.characters[7] = LCD__L;
  lcd_state.characters[8] = LCD__O;

  lcd_state.characters[20] = LCD__W;
  lcd_state.characters[21] = LCD__O;
  lcd_state.characters[22] = LCD__R;
  lcd_state.characters[23] = LCD__L;
  lcd_state.characters[24] = LCD__D;

  // TODO don't start automatically
  headwater_state_play(&headwater_api.state);

  atmega_lcd_send_cmd(0x0C); // turn on display
  _delay_ms(5);
  atmega_lcd_send_cmd(0x01); // clear display
  _delay_ms(5);
  atmega_lcd_send_cmd(0x3C); // function set
  _delay_ms(5);
  atmega_lcd_send_cmd(0x80);
  _delay_ms(5);

  // FIXME DEBUG
  lcd_state.mode = LCD_MODE_READ;

  /* atmega_lcd_send(0, 0x84); */
  /* atmega_lcd_send(1, LCD__A); */
  /* atmega_lcd_send(1, LCD__B); */
  /* atmega_lcd_send(1, LCD__C); */

  // enable interrupts
  sei();

  while(1) {
    if(headwater_api.state.change_flags != 0) {
      // state change
      headwater_state_change(&headwater_api.state);
    } else if(lcd_state.mode == LCD_MODE_WRITE) {
      lcd_state.characters[4] = LCD__G;
      lcd_state.characters[5] = LCD__O;
      lcd_state.characters[6] = LCD__O;
      lcd_state.characters[7] = LCD__D;
      lcd_state.characters[8] = LCD__B;
      lcd_state.characters[9] = LCD__Y;
      lcd_state.characters[10] = LCD__E;

      _delay_ms(2000);

      // DEBUG
      lcd_state.mode = LCD_MODE_READ;
      /* // lcd change */
      /* LCDField changed_field = lcd_next_changed_field(&lcd_screen_main); */
      /* if(lcd_is_field_null(changed_field)) { */
      /*   // no more changes, switch lcd modes */
      /*   lcd_state.mode = LCD_MODE_READ; */
      /* } else { */
      /*   // TODO write character changes for field */
      /*   headwater_lcd_write_main_field( */
      /*     changed_field, */
      /*     &headwater_api->state, */
      /*     &lcd_state.characters */
      /*   ); */
      /* } */
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

void atmega_headwater_lcd_read(LCD *lcd) {
  LCDCommand command = lcd_next_command(lcd);
  atmega_lcd_send(command.rs, command.data);
}

ISR(TIMER0_COMPA_vect) {
  if(lcd_state.mode == LCD_MODE_READ) {
    atmega_headwater_lcd_read(&lcd_state);
  } else if(lcd_state.mode == LCD_MODE_WAIT) {
    lcd_wait(&lcd_state);
  }

  /* atmega_lcd_set_prescaler(lcd_state.mode); */
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
