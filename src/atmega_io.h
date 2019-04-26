#ifndef _ATMEGA_IO_H_
#define _ATMEGA_IO_H_

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

void atmega_io_bpm_output(uint8_t enabled);
void atmega_io_multiplier_a_output(uint8_t enabled);
void atmega_io_multiplier_b_output(uint8_t enabled);
uint8_t atmega_io_sample_inputs(uint8_t count);
void atmega_io_init(void);

#endif
