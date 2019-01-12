#include <stdio.h>
#include "unity.h"
#include "unity_fixture.h"

#include "headwater_state.h"

TEST_GROUP(headwater_state);

HeadwaterState dummy_state;

TEST_SETUP(headwater_state) {
  dummy_state = headwater_state_new();
  dummy_state.output_enabled = 1;
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
  int16_t dummy_bpm = 3000;
  int8_t dummy_multiplier_a = 5;
  uint16_t dummy_sample_count = 32;
  uint16_t dummy_multiplier_count = 21;

  dummy_state.bpm_channel.samples = dummy_sample_count;
  dummy_state.multiplier_a_channel.beats = dummy_multiplier_count;

  headwater_state_update(&dummy_state, dummy_bpm, dummy_multiplier_a);

  TEST_ASSERT_EQUAL(dummy_bpm, dummy_state.bpm);
  TEST_ASSERT_EQUAL(dummy_multiplier_a, dummy_state.multiplier_a);
  TEST_ASSERT_EQUAL(200, dummy_state.bpm_channel.samples_per_beat);
  TEST_ASSERT_EQUAL(40, dummy_state.multiplier_a_channel.samples_per_beat);
  TEST_ASSERT_EQUAL(dummy_sample_count, dummy_state.bpm_channel.samples);
  TEST_ASSERT_EQUAL(
    dummy_multiplier_count,
    dummy_state.multiplier_a_channel.beats
  );
}

TEST(headwater_state, test_headwater_state_update_2) {
  int16_t dummy_bpm = 600;
  int8_t dummy_multiplier_a = 4;

  headwater_state_update(&dummy_state, dummy_bpm, dummy_multiplier_a);

  TEST_ASSERT_EQUAL(dummy_bpm, dummy_state.bpm);
  TEST_ASSERT_EQUAL(dummy_multiplier_a, dummy_state.multiplier_a);
  TEST_ASSERT_EQUAL(1000, dummy_state.bpm_channel.samples_per_beat);
  TEST_ASSERT_EQUAL(250, dummy_state.multiplier_a_channel.samples_per_beat);
}

TEST(headwater_state, test_headwater_state_increment_counts) {
  dummy_state.bpm_channel.samples_per_beat = 3;
  TEST_ASSERT_EQUAL(0, dummy_state.bpm_channel.samples);
  headwater_state_increment_counts(&dummy_state);
  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.samples);
  headwater_state_increment_counts(&dummy_state);
  TEST_ASSERT_EQUAL(2, dummy_state.bpm_channel.samples);
  headwater_state_increment_counts(&dummy_state);
  TEST_ASSERT_EQUAL(0, dummy_state.bpm_channel.samples);
  headwater_state_increment_counts(&dummy_state);
  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.samples);
}

TEST(headwater_state, test_headwater_state_cycle) {
  dummy_state.multiplier_a = 3;
  dummy_state.bpm_channel.samples_per_beat = 7;
  dummy_state.multiplier_a_channel.samples_per_beat = 2;

  int8_t expected_outputs[9][2] = {
    {1, 1},
    {0, 0},
    {0, 1},
    {0, 0},
    {0, 1},
    {0, 0},
    {0, 0},
    {1, 1},
    {0, 0}
  };

  for(uint8_t i = 0; i < 9; i++) {
    headwater_state_cycle(&dummy_state);

    TEST_ASSERT_EQUAL_MESSAGE(
      expected_outputs[i][0],
      dummy_state.bpm_channel.output,
      "Bad bpm output"
    );

    TEST_ASSERT_EQUAL_MESSAGE(
      expected_outputs[i][1],
      dummy_state.multiplier_a_channel.output,
      "Bad multiplier a output"
    );
  }
}

TEST(headwater_state, test_headwater_state_stop) {
  headwater_state_cycle(&dummy_state);

  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.output);
  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.samples);

  headwater_state_cycle(&dummy_state);

  TEST_ASSERT_EQUAL(0, dummy_state.bpm_channel.output);
  TEST_ASSERT_EQUAL(2, dummy_state.bpm_channel.samples);

  headwater_state_stop(&dummy_state);

  for(uint8_t i; i < 10; i++) {
    headwater_state_cycle(&dummy_state);

    TEST_ASSERT_EQUAL(0, dummy_state.bpm_channel.output);
    TEST_ASSERT_EQUAL(2, dummy_state.bpm_channel.samples);
  }

  headwater_state_start(&dummy_state);

  headwater_state_cycle(&dummy_state);

  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.output);
  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.samples);
}

TEST_GROUP_RUNNER(headwater_state) {
  RUN_TEST_CASE(headwater_state, test_headwater_state_samples_per_bpm);
  RUN_TEST_CASE(headwater_state, test_headwater_state_update);
  RUN_TEST_CASE(headwater_state, test_headwater_state_update_2);
  RUN_TEST_CASE(headwater_state, test_headwater_state_increment_counts);
  RUN_TEST_CASE(headwater_state, test_headwater_state_cycle);
  RUN_TEST_CASE(headwater_state, test_headwater_state_stop);
}