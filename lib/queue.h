#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "stdint.h"

typedef enum {
  QUEUE_EMPTY_FALSE,
  QUEUE_EMPTY_TRUE
} QUEUE_EMPTY;

typedef volatile struct QueueFifo255 {
  uint8_t count;
  uint8_t head;
  uint8_t items[256];
} QueueFifo255;

QueueFifo255 queue_fifo255_new(void);
QUEUE_EMPTY queue_fifo255_is_empty(QueueFifo255 *queue);
void queue_fifo255_push(QueueFifo255 *queue, uint8_t data);
uint8_t queue_fifo255_peek(QueueFifo255 *queue);
uint8_t queue_fifo255_pop(QueueFifo255 *queue);

#endif
