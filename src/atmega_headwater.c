#define F_CPU 16000000UL  // 16 MHz

#include "stdint.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "atmega_lcd.h"
#include "atmega_spi.h"
#include "bytes.h"
#include "debounce.h"
#include "headwater_api.h"
#include "headwater_lcd.h"
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

volatile API headwater_api;
volatile LCD lcd_state;
volatile UIScreen main_screen;

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

int main(void) {
  // enable output pins
  OUTPUT_DDR |= (1 << BPM_PIN) | (1 << MULTIPLIER_A_PIN);

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

  // TODO do this automatically elsewhere
  atmega_headwater_bpm_output(0);
  atmega_headwater_multiplier_a_output(0);

  headwater_api.payload_preprocessor = &headwater_api_payload_preprocessor;
  headwater_api.payload_postprocessor = &headwater_api_payload_postprocessor;
  headwater_api.request = api_new_request();

  // TODO pull state out of api
  headwater_api.state = headwater_state_new();

  atmega_lcd_init();
  atmega_spi_slave_init();

  // wait for LCD
  _delay_ms(500);

  // TODO create lcd state, screens
  LCD lcd_state_ = lcd_new();
  lcd_state = lcd_state_;

  /* headwater_lcd_update_main(&lcd_state, &headwater_api.state); */

  // TODO don't start automatically
  headwater_state_play(&headwater_api.state);

  // TODO do this in setup function
  atmega_lcd_send_cmd(0x0E); // turn on display and cursor
  _delay_ms(5);
  atmega_lcd_send_cmd(0x01); // clear display
  _delay_ms(5);
  atmega_lcd_send_cmd(0x3C); // function set
  _delay_ms(5);
  atmega_lcd_send_cmd(0x80); // move to start
  _delay_ms(5);

  // TODO move debounce setup?
  uint8_t debounce_threshold = 5;

  DebounceButton stop_button = debounce_button_new(
    DEBOUNCE_BUTTON_STATE_HIGH,
    debounce_threshold
  );

  DebounceButton play_button = debounce_button_new(
    DEBOUNCE_BUTTON_STATE_HIGH,
    debounce_threshold
  );

  DebounceEncoder rotary_encoder = debounce_encoder_new(
    DEBOUNCE_BUTTON_STATE_HIGH,
    debounce_threshold
  );

  DebounceButton rotary_encoder_button = debounce_button_new(
    DEBOUNCE_BUTTON_STATE_HIGH,
    debounce_threshold
  );

  DebounceButton left_button = debounce_button_new(
    DEBOUNCE_BUTTON_STATE_HIGH,
    debounce_threshold
  );

  DebounceButton right_button = debounce_button_new(
    DEBOUNCE_BUTTON_STATE_HIGH,
    debounce_threshold
  );

  // TODO move ui setup?
  main_screen = headwater_ui_main_screen(&headwater_api.state, &lcd_state);
  lcd_state.selected_position = ui_selected_position(&main_screen);

  // enable interrupts
  sei();

  while(1) {
    if(headwater_api.state.change_flags != 0) {
      headwater_state_change(&headwater_api.state);

    } else if(lcd_state.mode == LCD_MODE_WRITE) {
      // also... need to only update on change to avoid cursor flicker
      /* ui_update_selected_display(&main_screen); */

      // TODO
      if(main_screen.change_flags != 0) {
        ui_update_changed_display(&main_screen);
      }
      //

      if(ui_selected_modifier(&main_screen) == 0) {
        atmega_lcd_send_cmd(0x0E); // cursor only
      } else {
        atmega_lcd_send_cmd(0x0F); // cursor blinking
      }

      // TODO do this in headwater_lcd_update_main?
      lcd_state.mode = LCD_MODE_READ;
    } else {
      // TODO move input scanning to an interrupt (after sample interrupt?)
      // use this to implement
      // - button hold time
      // - asynchronous (get_change to get and clear latest change)
      // - proper timing for debounce and hold

      // snapshot inputs on falling edge of shift
      // UI_INPUT_PIN (Q) immediately set to H input
      UI_PORT &= ~(1 << UI_SHIFT_PIN);
      UI_PORT |= (1 << UI_SHIFT_PIN);

      uint8_t stop_value = (UI_PIN & (1 << UI_INPUT_PIN));
      UI_PORT |= (1 << UI_CLK_PIN);
      UI_PORT &= ~(1 << UI_CLK_PIN);
      uint8_t play_value = (UI_PIN & (1 << UI_INPUT_PIN));
      UI_PORT |= (1 << UI_CLK_PIN);
      UI_PORT &= ~(1 << UI_CLK_PIN);
      uint8_t re_a_value = (UI_PIN & (1 << UI_INPUT_PIN));
      UI_PORT |= (1 << UI_CLK_PIN);
      UI_PORT &= ~(1 << UI_CLK_PIN);
      uint8_t re_b_value = (UI_PIN & (1 << UI_INPUT_PIN));
      UI_PORT |= (1 << UI_CLK_PIN);
      UI_PORT &= ~(1 << UI_CLK_PIN);
      uint8_t re_sw_value = (UI_PIN & (1 << UI_INPUT_PIN));
      UI_PORT |= (1 << UI_CLK_PIN);
      UI_PORT &= ~(1 << UI_CLK_PIN);
      uint8_t left_value = (UI_PIN & (1 << UI_INPUT_PIN));
      UI_PORT |= (1 << UI_CLK_PIN);
      UI_PORT &= ~(1 << UI_CLK_PIN);
      uint8_t right_value = (UI_PIN & (1 << UI_INPUT_PIN));

      debounce_button_update(&stop_button, stop_value);
      debounce_button_update(&play_button, play_value);
      debounce_encoder_update(&rotary_encoder, re_a_value, re_b_value);
      debounce_button_update(&rotary_encoder_button, re_sw_value);
      debounce_button_update(&left_button, left_value);
      debounce_button_update(&right_button, right_value);

      if(stop_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        headwater_api.state.change_flags |=
          (1 << HEADWATER_STATE_CHANGE_STOP);
      }

      if(play_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        headwater_api.state.change_flags |=
          (1 << HEADWATER_STATE_CHANGE_PLAY);
      }

      if(rotary_encoder.output == DEBOUNCE_ENCODER_OUTPUT_LEFT) {
        ui_update_selected_modifier(&main_screen, -1);

        /* headwater_api.state.bpm -= 1; */
        /* headwater_api.state.change_flags |= */
        /*   (1 << HEADWATER_STATE_CHANGE_BPM); */
      }

      if(rotary_encoder.output == DEBOUNCE_ENCODER_OUTPUT_RIGHT) {
        ui_update_selected_modifier(&main_screen, 1);

        /* headwater_api.state.bpm += 1; */
        /* headwater_api.state.change_flags |= */
        /*   (1 << HEADWATER_STATE_CHANGE_BPM); */
      }

      // TODO handle long and short pushes
      if(rotary_encoder_button.change == DEBOUNCE_BUTTON_CHANGE_HIGH) {
        if(rotary_encoder_button.hold_count < 30000) {
          ui_update_selected_modifier(&main_screen, 1);
        } else {
          ui_update_selected_modifier(&main_screen, -1);
        }
        /* ui_update_selected_state(&main_screen); */
      }

      if(left_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        // TODO shift editing field to left
        ui_move_selected(&main_screen, UI_SCREEN_DIRECTION_DEC);
        lcd_state.selected_position = ui_selected_position(&main_screen);
      }

      if(right_button.change == DEBOUNCE_BUTTON_CHANGE_LOW) {
        // TODO shift editing field to right
        ui_move_selected(&main_screen, UI_SCREEN_DIRECTION_INC);
        lcd_state.selected_position = ui_selected_position(&main_screen);
      }
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

ISR(TIMER0_COMPA_vect) {
  // TODO move this into lcd module as lcd_handle_interrupt
  if(lcd_state.mode == LCD_MODE_READ) {
    LCDCommand command = lcd_next_command(&lcd_state);
    atmega_lcd_send(command.rs, command.data);
  } else if(lcd_state.mode == LCD_MODE_WAIT) {
    lcd_wait(&lcd_state);
  }

  // Reduce interrupt rate when not reading
  if(lcd_state.mode == LCD_MODE_READ) {
    OCR0A = 1;
  } else {
    OCR0A = 156;
  }
}

ISR(SPI_STC_vect) {
  SPDR = api_handle_interrupt(&headwater_api, SPDR);
}
