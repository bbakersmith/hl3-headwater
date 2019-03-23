#include "stdint.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#include "atmega_eeprom.h"
#include "atmega_lcd.h"
#include "atmega_spi.h"
#include "bytes.h"
#include "debounce.h"
#include "headwater_api.h"
#include "headwater_state.h"
#include "headwater_ui.h"
#include "lcd.h"
#include "ui.h"

#define UI_PIN PIND
#define UI_PORT PORTD
#define UI_DDR DDRD

#define UI_INPUT_PIN PORTD0
#define UI_CLK_PIN PORTD1
#define UI_SHIFT_PIN PORTD2

#define OUTPUT_PORT PORTC
#define OUTPUT_DDR DDRC

#define BPM_PIN PORTC0
#define MULTIPLIER_A_PIN PORTC1
#define MULTIPLIER_B_PIN PORTC2

volatile API api;
volatile HeadwaterState state;
volatile HeadwaterUIInputs inputs;
volatile LCD lcd;
volatile UIScreen screen;

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

void atmega_headwater_multiplier_b_output(uint8_t enabled) {
  if(enabled == 0) {
    OUTPUT_PORT |= (1 << MULTIPLIER_B_PIN);
  } else {
    OUTPUT_PORT &= ~(1 << MULTIPLIER_B_PIN);
  }
}

void atmega_headwater_global_register_setup(void) {
  // disable outputs
  atmega_headwater_bpm_output(0);
  atmega_headwater_multiplier_a_output(0);
  atmega_headwater_multiplier_b_output(0);

  // enable output pins
  OUTPUT_DDR |=
    (1 << BPM_PIN)
    | (1 << MULTIPLIER_A_PIN)
    | (1 << MULTIPLIER_B_PIN);

  // enable ui input
  UI_PORT |= (1 << UI_INPUT_PIN);

  // enable ui output
  UI_DDR |= (1<< UI_SHIFT_PIN) | (1 << UI_CLK_PIN);

  // timer clear timer on compare (ctc) mode
  TCCR0A |= (1 << WGM01);
  TCCR1B |= (1 << WGM12);

  // timer prescale
  TCCR0B |= (1 << CS02) | (1 << CS00); // 1/1024
  TCCR1B |= (1 << CS11);

  // timer reset value
  OCR0A = 156;
  OCR1A = 2000;

  // configure interrupts
  TIMSK0 |= (1 << OCIE0A);
  TIMSK1 |= (1 << OCIE1A);

  atmega_lcd_init();
  atmega_spi_slave_init();
}

void atmega_headwater_global_state_setup(void) {
  state = headwater_state_new();
  // TODO don't start automatically
  /* headwater_state_play(&state); */

  api.payload_preprocessor = &headwater_api_payload_preprocessor;
  api.payload_postprocessor = &headwater_api_payload_postprocessor;
  api.request = api_new_request();
  api.state = &state;

  lcd = lcd_new();
  screen = headwater_ui_main_screen(&state, &lcd);
  lcd.selected_position = ui_selected_position(&screen);

  // load preset 0
  headwater_ui_load_preset(&screen, &state, &atmega_eeprom_read);

  inputs = headwater_ui_inputs_new();
}

uint8_t atmega_headwater_sample_inputs(uint8_t count) {
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

int main(void) {
  atmega_headwater_global_register_setup();
  atmega_headwater_global_state_setup();

  sei(); // enable interrupts

  while(1) {
    if(state.change_flags != 0) {
      headwater_state_change(&state);
    } else if(lcd.mode == LCD_MODE_WRITE && ui_is_display_changed(&screen)) {
      ui_update_changed_display(&screen);

      // TODO only send if changed?
      if(ui_selected_modifier(&screen) == 0) {
        atmega_lcd_send_cmd(0x0E); // cursor only
      } else {
        atmega_lcd_send_cmd(0x0F); // cursor blinking
      }

      lcd.mode = LCD_MODE_READ;
    } else {
      headwater_ui_handle_inputs(
        &inputs,
        atmega_headwater_sample_inputs(8),
        &screen,
        &state,
        &atmega_eeprom_read,
        &atmega_eeprom_write
      );
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  headwater_state_cycle(&state);

  atmega_headwater_bpm_output(state.bpm_channel.output);
  atmega_headwater_multiplier_a_output(state.multiplier_a_channel.output);
  atmega_headwater_multiplier_b_output(state.multiplier_b_channel.output);
}

ISR(TIMER0_COMPA_vect) {
  lcd_handle_interrupt(&lcd, &atmega_lcd_send);

  // Reduce interrupt rate when not reading
  if(lcd.mode == LCD_MODE_READ) {
    OCR0A = 1;
  } else {
    OCR0A = 156;
  }
}

ISR(SPI_STC_vect) {
  SPDR = api_handle_interrupt(&api, SPDR);
}
