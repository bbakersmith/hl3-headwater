/**
 * @file atmega_io.h
 *
 * Headwater GPIO setup and read / write functions for the Atmega328p.
 */

#ifndef _ATMEGA_IO_H_
#define _ATMEGA_IO_H_

#include "stdbool.h"
#include "stdint.h"
#include <avr/io.h>

#include "bytes.h"

#define UI_PIN PIND
#define UI_PORT PORTD
#define UI_DDR DDRD

#define UI_INPUT_PIN PORTD2
#define UI_CLK_PIN PORTD3
#define UI_SHIFT_PIN PORTD4

#define OUTPUT_PORT PORTC
#define OUTPUT_DDR DDRC

#define BPM_PIN PORTC0
#define MULTIPLIER_A_PIN PORTC1
#define MULTIPLIER_B_PIN PORTC2
#define STOP_PLAY_PIN PORTC4

/**
 * Set state of BPM output pin.
 */
void atmega_io_bpm_output(bool enabled);

/**
 * Set state of multiplier A output pin.
 */
void atmega_io_multiplier_a_output(bool enabled);

/**
 * Set state of multiplier A output pin.
 */
void atmega_io_multiplier_b_output(bool enabled);

/**
 * Set state of stop / play LED indicator pin.
 */
void atmega_io_stop_play_output(bool enabled);

/**
 * Read inputs from parallel to serial shift register.
 *
 * @param Number of inputs to sample, max 8
 *
 * @return Byte with each bit representing the state of an input
 */
uint8_t atmega_io_sample_inputs(uint8_t count);

/**
 * Initialize GPIO for outputs and parallel to serial shift register.
 */
void atmega_io_init(void);

#endif
