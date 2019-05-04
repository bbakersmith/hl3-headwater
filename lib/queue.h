#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "stdint.h"

typedef struct QueueFifo255 {
  uint8_t count;
  uint8_t head;
  uint8_t items[255];
} QueueFifo255;

QueueFifo255 queue_fifo255_new(void);
void queue_fifo255_add(QueueFifo255 *queue, uint8_t data);
uint8_t queue_fifo255_remove(QueueFifo255 *queue);

#endif
