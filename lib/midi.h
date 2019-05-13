/**
 * @file midi.h
 *
 * MIDI read / write library with buffering, intended for use with system
 * specific serial read / write functions.
 */

#ifndef _MIDI_H_
#define _MIDI_H_

#include "stdbool.h"
#include "stdint.h"
#include "queue.h"

#define MIDI_ACTIVE_SENSING 0xFE
#define MIDI_CLOCK 0xF8
#define MIDI_CONTINUE 0xFB
#define MIDI_START 0xFA
#define MIDI_STOP 0xFC

// TODO should just be bool if only two states
typedef enum {
  MIDI_WRITER_STATUS_READY,
  MIDI_WRITER_STATUS_BUSY
} MIDIWriterStatus;

/**
 * MIDI state holding serial read / write functions and message buffers.
 */
typedef volatile struct MIDI {
  QueueFifo255 write_queue;
  void (*writer)(uint8_t data);
  bool (*writer_status_check)(void);
} MIDI;

/**
 * Create a new MIDI state, requires initialization of read / write functions
 * afterwards, as neither is strictly required.
 *
 * @return MIDI state
 */
MIDI midi_new(void);

/**
 * Read byte, no buffering.
 */
void midi_read(MIDI *midi);

/**
 * Queue byte for output, write immediately if writer not busy.
 */
void midi_write(MIDI *midi, uint8_t data);

/**
 * Write next item in queue if queue not empty and writer not busy.
 */
void midi_write_queue(MIDI *midi);

#endif
