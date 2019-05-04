#include <stdio.h>

#include "queue.h"

QueueFifo255 queue_fifo255_new(void) {
  QueueFifo255 queue = {
    count: 0,
    head: 0
  };
  return queue;
}

void queue_fifo255_add(QueueFifo255 *queue, uint8_t data) {
  uint8_t next_index = queue->head + queue->count;
  queue->items[next_index] = data;
  if(queue->count < 255) {
    queue->count++;
  } else {
    queue->head++;
  }
}

uint8_t queue_fifo255_remove(QueueFifo255 *queue) {
  uint8_t data = queue->items[queue->head];
  queue->head++;
  queue->count--;
  return data;
}
