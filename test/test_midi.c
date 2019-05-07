#include "midi.h"
#include "unity.h"
#include "unity_fixture.h"

MIDI dummy_midi;

uint8_t dummy_read_data;
uint8_t dummy_writes_count;
uint8_t dummy_writes[100];
MIDI_WRITER_STATUS dummy_writer_status = MIDI_WRITER_STATUS_READY;

uint8_t dummy_read_fn(void) {
  return dummy_read_data;
}

void dummy_write_fn(uint8_t data) {
  dummy_writes[dummy_writes_count] = data;
  dummy_writes_count++;
}

uint8_t dummy_writer_status_check_fn(void) {
  return dummy_writer_status;
}

TEST_GROUP(midi);

TEST_SETUP(midi) {
  dummy_midi = midi_new();
  /* dummy_midi.reader = &dummy_read_fn; */
  dummy_midi.writer = &dummy_write_fn;
  dummy_midi.writer_status_check = &dummy_writer_status_check_fn;

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
    {MIDI_WRITER_STATUS_READY, dummy_data_1, 1},
    {MIDI_WRITER_STATUS_BUSY, dummy_data_2, 1},
    {MIDI_WRITER_STATUS_BUSY, dummy_data_3, 1},
    {MIDI_WRITER_STATUS_READY, magic_dont_write_value, 2},
    {MIDI_WRITER_STATUS_READY, magic_dont_write_value, 3},
    {MIDI_WRITER_STATUS_BUSY, dummy_data_4, 3}
  };

  uint8_t message[50];
  for(uint8_t i = 0; i < 6; i ++) {
    dummy_writer_status = test_cases[i][0];

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
