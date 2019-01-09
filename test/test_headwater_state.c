#include "unity.h"
#include "unity_fixture.h"

#include "headwater_state.h"

TEST_GROUP(headwater_state);

HeadwaterState dummy_state;
uint8_t dummy_output;
uint8_t dummy_multiplied;

void dummy_update_output(uint8_t enabled) {
  dummy_output = enabled;
}

void dummy_update_multiplied(uint8_t enabled) {
  dummy_multiplied = enabled;
}

TEST_SETUP(headwater_state) {
  dummy_state = create_headwater_state();
  dummy_state.running = 1;
  dummy_output = 0;
  dummy_multiplied = 0;
}

TEST_TEAR_DOWN(headwater_state) {}

// Tests

TEST(headwater_state, test_headwater_state_samples_per_bpm) {
  TEST_ASSERT_EQUAL(1000, headwater_state_samples_per_bpm(600));
  TEST_ASSERT_EQUAL(500, headwater_state_samples_per_bpm(1200));
  TEST_ASSERT_EQUAL(250, headwater_state_samples_per_bpm(2400));
  TEST_ASSERT_EQUAL(200, headwater_state_samples_per_bpm(3000));
  TEST_ASSERT_EQUAL(497, headwater_state_samples_per_bpm(1205));
  TEST_ASSERT_EQUAL(60000, headwater_state_samples_per_bpm(10));
}

TEST(headwater_state, test_headwater_state_update) {
  int16_t dummy_tbpm = 3000;
  int8_t dummy_multiplier = 5;
  uint16_t dummy_sample_count = 32;
  uint16_t dummy_multiplied_count = 21;

  dummy_state.sample_count = dummy_sample_count;
  dummy_state.multiplied_count = dummy_multiplied_count;

  headwater_state_update(&dummy_state, dummy_tbpm, dummy_multiplier);

  TEST_ASSERT_EQUAL(dummy_tbpm, dummy_state.tbpm);
  TEST_ASSERT_EQUAL(dummy_multiplier, dummy_state.multiplier);
  TEST_ASSERT_EQUAL(200, dummy_state.samples_per_bpm);
  TEST_ASSERT_EQUAL(40, dummy_state.samples_per_multiplied);
  TEST_ASSERT_EQUAL(dummy_sample_count, dummy_state.sample_count);
  TEST_ASSERT_EQUAL(dummy_multiplied_count, dummy_state.multiplied_count);
}

TEST(headwater_state, test_headwater_state_update_2) {
  int16_t dummy_tbpm = 600;
  int8_t dummy_multiplier = 4;

  headwater_state_update(&dummy_state, dummy_tbpm, dummy_multiplier);

  TEST_ASSERT_EQUAL(dummy_tbpm, dummy_state.tbpm);
  TEST_ASSERT_EQUAL(dummy_multiplier, dummy_state.multiplier);
  TEST_ASSERT_EQUAL(1000, dummy_state.samples_per_bpm);
  TEST_ASSERT_EQUAL(250, dummy_state.samples_per_multiplied);
}

TEST(headwater_state, test_headwater_state_increment_counts) {
  dummy_state.samples_per_bpm = 3;
  TEST_ASSERT_EQUAL(0, dummy_state.sample_count);
  headwater_state_increment_counts(&dummy_state);
  TEST_ASSERT_EQUAL(1, dummy_state.sample_count);
  headwater_state_increment_counts(&dummy_state);
  TEST_ASSERT_EQUAL(2, dummy_state.sample_count);
  headwater_state_increment_counts(&dummy_state);
  TEST_ASSERT_EQUAL(0, dummy_state.sample_count);
  headwater_state_increment_counts(&dummy_state);
  TEST_ASSERT_EQUAL(1, dummy_state.sample_count);
}

TEST(headwater_state, test_headwater_state_cycle) {
  int8_t dummy_multiplier = 3;
  int16_t dummy_samples_per_bpm = 7;
  int8_t dummy_samples_per_multiplied = 2;

  dummy_state.multiplier = dummy_multiplier;
  dummy_state.samples_per_bpm = dummy_samples_per_bpm;
  dummy_state.samples_per_multiplied = dummy_samples_per_multiplied;

  int8_t expected_outputs[8][2] = {
    {1, 1},
    {0, 0},
    {0, 1},
    {0, 0},
    {0, 1},
    {0, 0},
    {0, 0},
    {1, 1}
  };

  for(uint8_t i = 0; i < 8; i++) {
    headwater_state_cycle(
        &dummy_state,
        &dummy_update_output,
        &dummy_update_multiplied
    );

    char fail_message[80];
    sprintf(
        fail_message,
        "Iteration %i failed, expected {%i, %i}, got {%i, %i}",
        i,
        expected_outputs[i][0],
        expected_outputs[i][1],
        dummy_output,
        dummy_multiplied
    );

    TEST_ASSERT_EQUAL_MESSAGE(
        expected_outputs[i][0],
        dummy_output,
        fail_message
    );

    TEST_ASSERT_EQUAL_MESSAGE(
        expected_outputs[i][1],
        dummy_multiplied,
        fail_message
    );
  }
}

TEST(headwater_state, test_headwater_state_stop) {
  headwater_state_cycle(
      &dummy_state,
      &dummy_update_output,
      &dummy_update_multiplied
  );

  TEST_ASSERT_EQUAL(1, dummy_output);
  TEST_ASSERT_EQUAL(1, dummy_output);
  TEST_ASSERT_EQUAL(1, dummy_state.sample_count);

  headwater_state_cycle(
      &dummy_state,
      &dummy_update_output,
      &dummy_update_multiplied
  );

  TEST_ASSERT_EQUAL(0, dummy_output);
  TEST_ASSERT_EQUAL(0, dummy_output);
  TEST_ASSERT_EQUAL(2, dummy_state.sample_count);

  headwater_state_stop(&dummy_state);

  for(uint8_t i; i < 10; i++) {
    headwater_state_cycle(
        &dummy_state,
        &dummy_update_output,
        &dummy_update_multiplied
    );

    TEST_ASSERT_EQUAL(0, dummy_output);
    TEST_ASSERT_EQUAL(0, dummy_output);
    TEST_ASSERT_EQUAL(2, dummy_state.sample_count);
  }

  headwater_state_start(&dummy_state);

  headwater_state_cycle(
      &dummy_state,
      &dummy_update_output,
      &dummy_update_multiplied
  );

  TEST_ASSERT_EQUAL(1, dummy_output);
  TEST_ASSERT_EQUAL(1, dummy_output);
  TEST_ASSERT_EQUAL(1, dummy_state.sample_count);
}

TEST_GROUP_RUNNER(headwater_state) {
  RUN_TEST_CASE(headwater_state, test_headwater_state_samples_per_bpm);
  RUN_TEST_CASE(headwater_state, test_headwater_state_update);
  RUN_TEST_CASE(headwater_state, test_headwater_state_update_2);
  RUN_TEST_CASE(headwater_state, test_headwater_state_increment_counts);
  RUN_TEST_CASE(headwater_state, test_headwater_state_cycle);
  RUN_TEST_CASE(headwater_state, test_headwater_state_stop);
}
