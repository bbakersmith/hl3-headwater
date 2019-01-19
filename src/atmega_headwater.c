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

API volatile headwater_api;
LCDState volatile lcd_state;

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
  TCCR1A |= (0 << WGM11) | (0 << WGM10);
  TCCR1B |= (0 << WGM13) | (1 << WGM12);

  // timer prescale
  TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);

  // timer reset value
  OCR1A = 2000;

  // enable interrupts
  TIMSK1 |= (1 << OCIE1A);
  PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);

  atmega_lcd_init();
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

  _delay_ms(100);

  // ?
  atmega_lcd_write_cmd(0x0C);
  atmega_lcd_write_cmd(0x01);
  atmega_lcd_write_cmd(0x38);

  LCDState lcd_state_ = {
    .index = 0,
    .write_cmd_fn = &atmega_lcd_write_cmd,
    .write_data_fn = &atmega_lcd_write_data
  };
  lcd_state = lcd_state_;

  LCD_CHAR message[16] = {
    LCD__H,
    LCD__E,
    LCD__L,
    LCD__L,
    LCD__O
  };

  lcd_state.screens[0] = lcd_screen_message_top_new(message, 5);

  /* atmega_lcd_write_data(0x80); */
  /*  */
  /* _delay_ms(1000); */

  atmega_lcd_write_cmd(LCD_ROW_1_COL_7);

  atmega_lcd_write_data(LCD__W);
  atmega_lcd_write_data(LCD__O);
  atmega_lcd_write_data(LCD__R);
  atmega_lcd_write_data(LCD__L);
  atmega_lcd_write_data(LCD__D);

  lcd_handle_interrupt(&lcd_state);
  lcd_handle_interrupt(&lcd_state);
  lcd_handle_interrupt(&lcd_state);
  lcd_handle_interrupt(&lcd_state);

  while(1) {
    if(headwater_api.state.change_flags == 0) {

      /* lcd_handle_interrupt(&lcd_state); */

      /* atmega_lcd_write_cmd(LCD_ROW_2_COL_4); */

      /* for(uint8_t i = 0; i < 5; i++) { */
      /*   atmega_lcd_write_data(lcd_state.screens[0].fields[0].value[i]); */
      /* } */



      // BEGIN BULLSHIT
      //
      //
      //
      //
      //


      /* LCDScreen *screen = &lcd_state.screens[lcd_state.index]; */
      /*  */
      /* // reset if all fields complete */
      /* if(screen->length <= screen->index) { */
      /*   screen->index = 0; */
      /* } */
      /*  */
      /* LCDScreenField *initial_field = &screen->fields[screen->index]; */
      /*  */
      /* // next field if current field complete */
      /* if(initial_field->length <= initial_field->index) { */
      /*   initial_field->index = -1; */
      /*   screen->index++; */
      /* } */
      /*  */
      /* LCDScreenField *field = &screen->fields[screen->index]; */
      /*  */
      /* // if new field move cursor, otherwise write data */
      /* if(field->index == -1) { */
      /*   atmega_lcd_write_cmd(field->position); */
      /*   #<{(| lcd_state.write_cmd_fn(field->position); |)}># */
      /* } else if(field->index < field->length) { */
      /*   atmega_lcd_write_data(field->value[field->index]); */
      /*   #<{(| lcd_state.write_data_fn(field->value[field->index]); |)}># */
      /* } */
      /*  */
      /* field->index++; */
      /*  */

      // END BULLSHIT







      _delay_ms(500);

      /* if(bug == 0) { */
      /*   lcd_state.screens[0].fields[0].value[0] = LCD__X; */
      /*   lcd_state.screens[0].fields[0].value[1] = LCD__Y; */
      /*   lcd_state.screens[0].fields[0].value[2] = LCD__Z; */
      /*   bug = 1; */
      /* } else { */
      /*   lcd_state.screens[0].fields[0].value[0] = LCD__H; */
      /*   lcd_state.screens[0].fields[0].value[1] = LCD__E; */
      /*   lcd_state.screens[0].fields[0].value[2] = LCD__L; */
      /*   bug = 0; */
      /* } */

      // TODO if lcd timer interrupt flag set...

      /* lcd_handle_interrupt(&lcd_state); */
      /* _delay_ms(500); */

      /* _delay_ms(500); */
      /*  */
      /* atmega_lcd_write_cmd(0xCD); */
      /*  */
      /* atmega_lcd_write_data(0x44); */
      /* atmega_lcd_write_data(0x45); */
      /* atmega_lcd_write_data(0x46); */

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
