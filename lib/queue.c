#include <stdio.h>

#include "queue.h"

QueueFifo255 queue_fifo255_new(void) {
  QueueFifo255 queue = {
    .count = 0,
    .head = 0
  };
  return queue;
}

bool queue_fifo255_is_empty(QueueFifo255 *queue) {
  return queue->count == 0;
}

void queue_fifo255_push(QueueFifo255 *queue, uint8_t data) {
  uint8_t next_index = queue->head + queue->count;
  queue->items[next_index] = data;
  if(queue->count < 255) {
    queue->count++;
  } else {
    queue->head++;
  }
}

uint8_t queue_fifo255_peek(QueueFifo255 *queue) {
  return queue->items[queue->head];
}

uint8_t queue_fifo255_pop(QueueFifo255 *queue) {
  uint8_t data = queue_fifo255_peek(queue);
  queue->head++;
  queue->count--;
  return data;
}
