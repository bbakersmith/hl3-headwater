/**
 * @file bytes.h
 *
 * @brief Functions for common bit and byte manipulation tasks.
 */

#ifndef _BYTES_H_
#define _BYTES_H_

#include "stdint.h"

/**
 * Get low byte of a uint16.
 */
uint8_t bytes_16bit_to_low(uint16_t data);

/**
 * Get high byte of a uint16.
 */
uint8_t bytes_16bit_to_high(uint16_t data);

/**
 * Create uint16 from its high and low bytes.
 */
uint16_t bytes_high_low_to_16bit(uint8_t high, uint8_t low);

/**
 * Set a single bit of a uint8.
 */
void bytes_set_bit(uint8_t volatile *value, uint8_t bit);

/**
 * Unset a single bit of a uint8.
 */
void bytes_unset_bit(uint8_t volatile *value, uint8_t bit);

/**
 * Return bit value of single bit of a uint8.
 */
uint8_t bytes_check_bit(uint8_t value, uint8_t bit);

#endif
