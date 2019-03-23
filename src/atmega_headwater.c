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
volatile HeadwaterState headwater_state;
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
  headwater_state = headwater_state_new();
  // TODO don't start automatically
  /* headwater_state_play(&headwater_state); */

  api.payload_preprocessor = &headwater_api_payload_preprocessor;
  api.payload_postprocessor = &headwater_api_payload_postprocessor;
  api.request = api_new_request();
  api.state = &headwater_state;

  screen = headwater_ui_main_screen(&headwater_state, &lcd);

  lcd = lcd_new();
  lcd.selected_position = ui_selected_position(&screen);

  // load preset 0
  headwater_ui_load_preset(&screen, &headwater_state, &atmega_eeprom_read);

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
    if(headwater_state.change_flags != 0) {
      headwater_state_change(&headwater_state);

    } else if(
      lcd.mode == LCD_MODE_WRITE
      && ui_is_display_changed(&screen)
    ) {
      ui_update_changed_display(&screen);

      // TODO only send if changed?
      if(ui_selected_modifier(&screen) == 0) {
        atmega_lcd_send_cmd(0x0E); // cursor only
      } else {
        atmega_lcd_send_cmd(0x0F); // cursor blinking
      }

      lcd.mode = LCD_MODE_READ;
    } else {
      // TODO move input scanning to an interrupt (after sample interrupt?)
      // use this to implement
      // - button hold time
      // - asynchronous (get_change to get and clear latest change)
      // - proper timing for debounce and hold

      uint8_t input_flags = atmega_headwater_sample_inputs(8);

      debounce_button_update(
        &inputs.stop_button,
        bytes_check_bit(input_flags, 0)
      );

      debounce_button_update(
        &inputs.play_button,
        bytes_check_bit(input_flags, 1)
      );

      debounce_encoder_update(
        &inputs.rotary_encoder,
        bytes_check_bit(input_flags, 2),
        bytes_check_bit(input_flags, 3)
      );

      debounce_button_update(
        &inputs.rotary_encoder_button,
        bytes_check_bit(input_flags, 4)
      );

      debounce_button_update(
        &inputs.left_button,
        bytes_check_bit(input_flags, 5)
      );

      debounce_button_update(
        &inputs.right_button,
        bytes_check_bit(input_flags, 6)
      );

      debounce_button_update(
        &inputs.save_button,
        bytes_check_bit(input_flags, 7)
      );

      if(inputs.stop_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        headwater_state.change_flags |= (1 << HEADWATER_STATE_CHANGE_STOP);
      }

      if(inputs.play_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        headwater_state.change_flags |= (1 << HEADWATER_STATE_CHANGE_PLAY);
      }

      if(inputs.rotary_encoder.output == DEBOUNCE_ENCODER_OUTPUT_LEFT) {
        ui_update_selected_modifier(&screen, -1);
      }

      if(inputs.rotary_encoder.output == DEBOUNCE_ENCODER_OUTPUT_RIGHT) {
        ui_update_selected_modifier(&screen, 1);
      }

      if(inputs.rotary_encoder_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        headwater_ui_update_selected_state(
          &screen,
          &headwater_state,
          &atmega_eeprom_read
        );
      }

      if(inputs.left_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        ui_move_selected(&screen, UI_SCREEN_DIRECTION_DEC);
        lcd.selected_position = ui_selected_position(&screen);
      }

      if(inputs.right_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        ui_move_selected(&screen, UI_SCREEN_DIRECTION_INC);
        lcd.selected_position = ui_selected_position(&screen);
      }

      if(inputs.save_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        headwater_ui_save_preset(
          &screen,
          &headwater_state,
          &atmega_eeprom_write
        );
      }
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  headwater_state_cycle(&headwater_state);

  atmega_headwater_bpm_output(headwater_state.bpm_channel.output);
  atmega_headwater_multiplier_a_output(headwater_state.multiplier_a_channel.output);
  atmega_headwater_multiplier_b_output(headwater_state.multiplier_b_channel.output);
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
