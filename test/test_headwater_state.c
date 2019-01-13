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

TEST(headwater_state, test_headwater_state_apply_modifier) {
  uint16_t result;

  result = headwater_state_apply_modifier(400, 51);
  TEST_ASSERT_EQUAL(2000, result);

  result = headwater_state_apply_modifier(400, 255);
  TEST_ASSERT_EQUAL(400, result);

  result = headwater_state_apply_modifier(10000, 255);
  TEST_ASSERT_EQUAL(10000, result);

  result = headwater_state_apply_modifier(30000, 127);
  TEST_ASSERT_EQUAL(60000, result);

  result = headwater_state_apply_modifier(2, 127);
  TEST_ASSERT_EQUAL(4, result);

  result = headwater_state_apply_modifier(400, 50);
  TEST_ASSERT_EQUAL(2040, result);

  result = headwater_state_apply_modifier(400, 52);
  TEST_ASSERT_EQUAL(1960, result);
}

TEST(headwater_state, test_headwater_state_update_samples_per_beat) {
  headwater_state_update_samples_per_beat(&dummy_state.bpm_channel, 30000);
  TEST_ASSERT_EQUAL(30000, dummy_state.bpm_channel.samples_per_beat);
}

TEST(headwater_state, test_headwater_state_update_samples_per_beat_modified) {
  dummy_state.multiplier_a_channel.modifier = 51;
  headwater_state_update_samples_per_beat(
    &dummy_state.multiplier_a_channel,
    500
  );
  TEST_ASSERT_EQUAL(2500, dummy_state.multiplier_a_channel.samples_per_beat);
}

TEST(
  headwater_state,
  test_headwater_state_update_samples_per_beat_multiplied
) {
  dummy_state.multiplier_a_channel.multiplier = 4;
  headwater_state_update_samples_per_beat(
    &dummy_state.multiplier_a_channel,
    2000
  );
  TEST_ASSERT_EQUAL(500, dummy_state.multiplier_a_channel.samples_per_beat);
}

TEST(
  headwater_state,
  test_headwater_state_update_samples_per_beat_modified_multiplied
) {
  dummy_state.multiplier_a_channel.modifier = 51;
  dummy_state.multiplier_a_channel.multiplier = 4;
  headwater_state_update_samples_per_beat(
    &dummy_state.multiplier_a_channel,
    400
  );
  TEST_ASSERT_EQUAL(500, dummy_state.multiplier_a_channel.samples_per_beat);
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
  dummy_state.multiplier_a_channel.multiplier = 3;
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

  headwater_state_play(&dummy_state);

  headwater_state_cycle(&dummy_state);

  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.output);
  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.samples);
}

TEST_GROUP_RUNNER(headwater_state) {
  RUN_TEST_CASE(headwater_state, test_headwater_state_samples_per_bpm);
  RUN_TEST_CASE(headwater_state, test_headwater_state_apply_modifier);
  RUN_TEST_CASE(headwater_state, test_headwater_state_update_samples_per_beat);
  RUN_TEST_CASE(
    headwater_state,
    test_headwater_state_update_samples_per_beat_modified
  );
  RUN_TEST_CASE(
    headwater_state,
    test_headwater_state_update_samples_per_beat_multiplied
  );
  RUN_TEST_CASE(
    headwater_state,
    test_headwater_state_update_samples_per_beat_modified_multiplied
  );
  RUN_TEST_CASE(headwater_state, test_headwater_state_increment_counts);
  RUN_TEST_CASE(headwater_state, test_headwater_state_cycle);
  RUN_TEST_CASE(headwater_state, test_headwater_state_stop);
}
