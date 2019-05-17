/**
 * @file main.c
 */

#include "stdint.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#include "atmega_eeprom.h"
#include "atmega_io.h"
#include "atmega_lcd.h"
#include "atmega_spi.h"
#include "atmega_uart.h"
#include "bytes.h"
#include "debounce.h"
#include "headwater_api.h"
#include "headwater_state.h"
#include "headwater_ui.h"
#include "lcd.h"
#include "midi.h"
#include "ui.h"

volatile API api;
volatile HeadwaterState state;
volatile HeadwaterUIInputs inputs;
volatile LCD lcd;
volatile MIDI midi;
volatile UIScreen screen;

/**
 * Initialize timers, interrupts, GPIO, and peripherals.
 */
void atmega_headwater_global_register_init(void) {
  // timer clear timer on compare (ctc) mode
  TCCR0A |= (1 << WGM01);
  TCCR1B |= (1 << WGM12);

  // timer prescale
  TCCR0B |= (1 << CS02) | (1 << CS00); // 1/1024
  TCCR1B |= (1 << CS11);               // 1/8

  // timer reset value
  OCR0A = 156;
  OCR1A = 2000;

  // configure interrupts
  TIMSK0 |= (1 << OCIE0A);
  TIMSK1 |= (1 << OCIE1A);

  atmega_io_init();
  atmega_lcd_init();
  atmega_spi_slave_init();
  atmega_uart_init();
}

/**
 * Initialize global state objects.
 */
void atmega_headwater_global_state_init(void) {
  midi = midi_new();
  midi.writer = &atmega_uart_writer;
  midi.writer_is_busy = &atmega_uart_is_busy;

  state = headwater_state_new();
  state.midi = &midi;

  api.payload_preprocessor = &headwater_api_payload_preprocessor;
  api.payload_postprocessor = &headwater_api_payload_postprocessor;
  api.request = api_request_new();
  api.state = &state;

  lcd = lcd_new();
  screen = headwater_ui_main_screen(&state, &lcd);
  lcd.selected_position = ui_selected_position(&screen);

  // load preset 0
  headwater_ui_load_preset(&screen, &state, &atmega_eeprom_read);

  inputs = headwater_ui_inputs_new();
}

/**
 * The program entrypoint, including initialization and the main loop.
 */
int main(void) {
  atmega_headwater_global_register_init();
  atmega_headwater_global_state_init();

  sei(); // enable interrupts

  while(1) {
    if(state.change_flags != 0) {
      headwater_state_handle_change_now(&state);
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
        atmega_io_sample_inputs(8),
        &screen,
        &state,
        &atmega_eeprom_read,
        &atmega_eeprom_write
      );
    }
  }
}

/**
 * Interrupt for refreshing the LCD display. Changes speed depending on LCD
 * mode.
 */
ISR(TIMER0_COMPA_vect) {
  lcd_handle_interrupt(&lcd, &atmega_lcd_send);

  // Reduce interrupt rate when not reading
  if(lcd.mode == LCD_MODE_READ) {
    OCR0A = 1;
  } else {
    OCR0A = 156;
  }
}

/**
 * Interrupt for updating HeadwaterState counts and outputs. This is the
 * heart of the Headwater system.
 */
ISR(TIMER1_COMPA_vect) {
  headwater_state_cycle(&state);

  // TODO FIXME update display after external and tap inputs
  // ...this may not be the right place
  if(
    state.mode == HEADWATER_STATE_MODE_EXTERNAL
    && state.bpm_channel.output != 0
  ) {
    // TODO fix magic number field index
    screen.change_flags |= (1 << 1);
  }

  atmega_io_bpm_output(state.bpm_channel.output);
  atmega_io_multiplier_a_output(state.multiplier_a_channel.output);
  atmega_io_multiplier_b_output(state.multiplier_b_channel.output);

  if(state.midi_channel.output)
    midi.writer(MIDI_CLOCK);
}

/**
 * Interrupt for reading and reacting to inbound SPI commands.
 */
ISR(SPI_STC_vect) {
  SPDR = api_handle_interrupt(&api, SPDR);
}

/**
 * Interrupt for reading inbound MIDI data.
 */
ISR(USART_RX_vect) {
  // TODO
  // if midi mode...
  // - midi_read
  //   - stop, start, continue
  //   - clock (fire play every 24th)
}

/**
 * Interrupt for writing outbound MIDI data.
 */
ISR(USART_TX_vect) {
  midi_write_queue(&midi);
}
