#include "midi.h"
#include "unity.h"
#include "unity_fixture.h"

MIDI dummy_midi;

uint8_t dummy_read_data;
uint8_t dummy_writes_count;
uint8_t dummy_writes[100];
bool dummy_writer_is_busy = false;

uint8_t dummy_read_fn(void) {
  return dummy_read_data;
}

void dummy_write_fn(uint8_t data) {
  dummy_writes[dummy_writes_count] = data;
  dummy_writes_count++;
}

bool dummy_writer_is_busy_fn(void) {
  return dummy_writer_is_busy;
}

TEST_GROUP(midi);

TEST_SETUP(midi) {
  dummy_midi = midi_new();
  /* dummy_midi.reader = &dummy_read_fn; */
  dummy_midi.writer = &dummy_write_fn;
  dummy_midi.writer_is_busy = &dummy_writer_is_busy_fn;

  dummy_writes_count = 0;
  for(uint8_t i = 0; i < 100; i++) {
    dummy_writes[i] = 0;
  }
}

TEST_TEAR_DOWN(midi) {}

TEST(midi, test_midi_write) {
  uint8_t dummy_data = 123;

  TEST_ASSERT_EQUAL(0, dummy_writes_count);

  midi_write(&dummy_midi, dummy_data);

  TEST_ASSERT_EQUAL(1, dummy_writes_count);
  TEST_ASSERT_EQUAL(dummy_data, dummy_writes[0]);
}

TEST(midi, test_midi_write_queue) {
  uint8_t dummy_data_1 = 11;
  uint8_t dummy_data_2 = 22;
  uint8_t dummy_data_3 = 33;
  uint8_t dummy_data_4 = 44;

  uint8_t magic_dont_write_value = 99;

  TEST_ASSERT_EQUAL(0, dummy_writes_count);

  uint8_t test_cases[6][3] = {
    {false, dummy_data_1, 1},
    {true, dummy_data_2, 1},
    {true, dummy_data_3, 1},
    {false, magic_dont_write_value, 2},
    {false, magic_dont_write_value, 3},
    {true, dummy_data_4, 3}
  };

  uint8_t message[50];
  for(uint8_t i = 0; i < 6; i ++) {
    dummy_writer_is_busy = test_cases[i][0];

    uint8_t data = test_cases[i][1];
    if(data != magic_dont_write_value)
      midi_write(&dummy_midi, data);

    sprintf(message, "Wrong count for iteration %i", i);
    midi_write_queue(&dummy_midi);
    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i][2], dummy_writes_count, message);
  }

  TEST_ASSERT_EQUAL(dummy_data_1, dummy_writes[0]);
  TEST_ASSERT_EQUAL(dummy_data_2, dummy_writes[1]);
  TEST_ASSERT_EQUAL(dummy_data_3, dummy_writes[2]);
  TEST_ASSERT_NOT_EQUAL(dummy_data_4, dummy_writes[3]);
}

/* TEST(midi, test_midi_read_systemrt) { */
/*   dummy_message = MIDI_START; */
/*  */
/*   MIDIMessage message = midi_read(&dummy_midi); */
/*  */
/*   TEST_ASSERT_EQUAL(MIDI_READER_STATUS_READY, message->reader_status); */
/*   TEST_ASSERT_EQUAL(dummy_message, message->status); */
/* } */


// TODO test midi reading, pg 5 of midi spec


TEST_GROUP_RUNNER(midi) {
  RUN_TEST_CASE(midi, test_midi_write);
  RUN_TEST_CASE(midi, test_midi_write_queue);
}
