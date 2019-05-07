#ifndef _MIDI_H_
#define _MIDI_H_

#include "stdint.h"
#include "queue.h"

#define MIDI_ACTIVE_SENSING 0xFE
#define MIDI_CLOCK 0xF8
#define MIDI_CONTINUE 0xFB
#define MIDI_START 0xFA
#define MIDI_STOP 0xFC

typedef enum {
  MIDI_WRITER_STATUS_READY,
  MIDI_WRITER_STATUS_BUSY
} MIDI_WRITER_STATUS;

typedef volatile struct MIDI {
  QueueFifo255 write_queue;
  void (*writer)(uint8_t data);
  uint8_t (*writer_status_check)(void);
} MIDI;

MIDI midi_new(void);
void midi_read(MIDI *midi);
void midi_write(MIDI *midi, uint8_t data);
void midi_write_queue(MIDI *midi);

#endif
