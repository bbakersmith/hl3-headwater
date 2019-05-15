#include "queue.h"
#include "unity.h"
#include "unity_fixture.h"

QueueFifo255 dummy_queue;

TEST_GROUP(queue);

TEST_SETUP(queue) {
  dummy_queue = queue_fifo255_new();
}

TEST_TEAR_DOWN(queue) {}

TEST(queue, test_queue_is_empty) {
  TEST_ASSERT_EQUAL(true, queue_fifo255_is_empty(&dummy_queue));
  TEST_ASSERT_EQUAL(true, queue_fifo255_is_empty(&dummy_queue));
  queue_fifo255_push(&dummy_queue, 123);
  TEST_ASSERT_EQUAL(false, queue_fifo255_is_empty(&dummy_queue));
}

TEST(queue, test_queue_push) {
  TEST_ASSERT_EQUAL(0, dummy_queue.count);
  queue_fifo255_push(&dummy_queue, 123);
  TEST_ASSERT_EQUAL(1, dummy_queue.count);
}

TEST(queue, test_queue_pop) {
  TEST_ASSERT_EQUAL(0, dummy_queue.count);
  queue_fifo255_push(&dummy_queue, 11);
  TEST_ASSERT_EQUAL(1, dummy_queue.count);
  queue_fifo255_push(&dummy_queue, 22);
  TEST_ASSERT_EQUAL(2, dummy_queue.count);
  TEST_ASSERT_EQUAL(11, queue_fifo255_pop(&dummy_queue));
  TEST_ASSERT_EQUAL(1, dummy_queue.count);
  TEST_ASSERT_EQUAL(22, queue_fifo255_pop(&dummy_queue));
  TEST_ASSERT_EQUAL(0, dummy_queue.count);
}

TEST(queue, test_queue_peek) {
  uint8_t dummy_data_1 = 122;
  uint8_t dummy_data_2 = 233;

  queue_fifo255_push(&dummy_queue, dummy_data_1);
  queue_fifo255_push(&dummy_queue, dummy_data_2);

  TEST_ASSERT_EQUAL(dummy_data_1, queue_fifo255_peek(&dummy_queue));
  TEST_ASSERT_EQUAL(dummy_data_1, queue_fifo255_peek(&dummy_queue));
  TEST_ASSERT_EQUAL(dummy_data_1, queue_fifo255_pop(&dummy_queue));
  TEST_ASSERT_EQUAL(dummy_data_2, queue_fifo255_peek(&dummy_queue));
}

TEST(queue, test_queue_push_rollover) {
  for(uint8_t i = 0; i < 255; i++) {
    TEST_ASSERT_EQUAL(i, dummy_queue.count);
    queue_fifo255_push(&dummy_queue, i);
    TEST_ASSERT_EQUAL(0, dummy_queue.head);
  }

  TEST_ASSERT_EQUAL(255, dummy_queue.count);

  for(uint8_t i = 0; i < 15; i++) {
    TEST_ASSERT_EQUAL(i, dummy_queue.head);
    queue_fifo255_push(&dummy_queue, (100 - i));
    TEST_ASSERT_EQUAL(255, dummy_queue.count);
  }

  TEST_ASSERT_EQUAL(15, dummy_queue.head);

  for(uint8_t i = 0; i < 240; i++) {
    TEST_ASSERT_EQUAL((i + 15), dummy_queue.head);
    TEST_ASSERT_EQUAL((i + 15), queue_fifo255_pop(&dummy_queue));
    TEST_ASSERT_EQUAL(254 - i, dummy_queue.count);
  }

  TEST_ASSERT_EQUAL(255, dummy_queue.head);

  for(uint8_t i = 0; i < 15; i++) {
    TEST_ASSERT_EQUAL((100 - i), queue_fifo255_pop(&dummy_queue));
    TEST_ASSERT_EQUAL(14 - i, dummy_queue.count);
    TEST_ASSERT_EQUAL(i, dummy_queue.head);
  }
}

TEST_GROUP_RUNNER(queue) {
  RUN_TEST_CASE(queue, test_queue_is_empty);
  RUN_TEST_CASE(queue, test_queue_push);
  RUN_TEST_CASE(queue, test_queue_pop);
  RUN_TEST_CASE(queue, test_queue_peek);
  RUN_TEST_CASE(queue, test_queue_push_rollover);
}
