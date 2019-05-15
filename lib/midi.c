#include <stdio.h>

#include "midi.h"

MIDI midi_new(void) {
  MIDI midi = {
    .write_queue = queue_fifo255_new()
  };
  return midi;
}

// MIDI_READER_READY
//
// - MIDI_READER_SYSTEM (MSB == 1)
//
// - MIDI_READER_CHANNEL (MSB == 0)
//

/* void midi_read(MIDI *state) { */
/*   uint8_t data = state->reader(); */
/*  */
/*   //  */
/*  */
/*   if(data >> 7) { */
/*     // status byte */
/*     if( */
/*   } else { */
/*     // data byte */
/*   } */
/*  */
/*   // if no data bytes */
/*   // if 1 data byte */
/*   // if 2 data bytes */
/* } */

void midi_write_queue(MIDI *midi) {
  if(
    !midi->writer_is_busy()
    && !queue_fifo255_is_empty(&midi->write_queue)
  ) {
    midi->writer(queue_fifo255_pop(&midi->write_queue));
  }
}

void midi_write(MIDI *midi, uint8_t data) {
  queue_fifo255_push(&midi->write_queue, data);
  midi_write_queue(midi);
}
