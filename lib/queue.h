/**
 * @file queue.h
 *
 * General purpose queue implementations.
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "stdbool.h"
#include "stdint.h"

/**
 * The contents and metadata of a circular FIFO queue with 255 values.
 */
typedef volatile struct QueueFifo255 {
  uint8_t count;
  uint8_t head;
  uint8_t items[256];
} QueueFifo255;

/**
 * Create a new circular FIFO queue with space reserved for 255 uint8 values.
 * Drops oldest value in queue on rollover.
 *
 * @return Queue
 */
QueueFifo255 queue_fifo255_new(void);

/**
 * Check if queue is empty.
 *
 * @return True if empty
 */
bool queue_fifo255_is_empty(QueueFifo255 *queue);

/**
 * Push value onto the back of the queue.
 */
void queue_fifo255_push(QueueFifo255 *queue, uint8_t data);

/**
 * Return the value at head without modifying the queue.
 *
 * @return Value at head of queue
 */
uint8_t queue_fifo255_peek(QueueFifo255 *queue);

/**
 * Return the value at head and move head to next value.
 *
 * @return Value at head of queue
 */
uint8_t queue_fifo255_pop(QueueFifo255 *queue);

#endif
