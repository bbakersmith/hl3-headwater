#include <stdio.h>
#include "unity.h"
#include "unity_fixture.h"

#include "headwater_state.h"

TEST_GROUP(headwater_state);

HeadwaterState dummy_state;

TEST_SETUP(headwater_state) {
  dummy_state = headwater_state_new();
  dummy_state.output_enabled = true;
}

TEST_TEAR_DOWN(headwater_state) {}

// Tests

TEST(headwater_state, test_headwater_state_samples_to_bpm) {
  TEST_ASSERT_EQUAL(600, headwater_state_samples_to_bpm(1000, 1000));
  TEST_ASSERT_EQUAL(1200, headwater_state_samples_to_bpm(1000, 500));
  TEST_ASSERT_EQUAL(2400, headwater_state_samples_to_bpm(1000, 250));
  TEST_ASSERT_EQUAL(3000, headwater_state_samples_to_bpm(1000, 200));
  TEST_ASSERT_EQUAL(1207, headwater_state_samples_to_bpm(1000, 497));
  TEST_ASSERT_EQUAL(10, headwater_state_samples_to_bpm(1000, 60000));
}

TEST(headwater_state, test_headwater_state_bpm_to_samples) {
  TEST_ASSERT_EQUAL(1000, headwater_state_bpm_to_samples(1000, 600));
  TEST_ASSERT_EQUAL(500, headwater_state_bpm_to_samples(1000, 1200));
  TEST_ASSERT_EQUAL(250, headwater_state_bpm_to_samples(1000, 2400));
  TEST_ASSERT_EQUAL(200, headwater_state_bpm_to_samples(1000, 3000));
  TEST_ASSERT_EQUAL(497, headwater_state_bpm_to_samples(1000, 1205));
  TEST_ASSERT_EQUAL(60000, headwater_state_bpm_to_samples(1000, 10));
}

TEST(headwater_state, test_headwater_state_update_samples_per_beat) {
  headwater_state_update_samples_per_beat(&dummy_state.bpm_channel, 30000);
  TEST_ASSERT_EQUAL(30000, dummy_state.bpm_channel.samples_per_beat);
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

TEST(headwater_state, test_headwater_state_increment_counts) {
  headwater_state_update_samples_per_beat(&dummy_state.bpm_channel, 3);
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
  }

  headwater_state_play(&dummy_state);

  headwater_state_cycle(&dummy_state);

  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.output);
  TEST_ASSERT_EQUAL(1, dummy_state.bpm_channel.samples);
}

TEST(headwater_state, test_headwater_state_cycle_internal) {
  dummy_state.mode = HEADWATER_STATE_MODE_INTERNAL;
  dummy_state.multiplier_a_channel.multiplier = 3;
  dummy_state.multiplier_b_channel.multiplier = 0;

  headwater_state_update_samples_per_beat(
    &dummy_state.bpm_channel,
    7
  );

  headwater_state_update_samples_per_beat(
    &dummy_state.multiplier_a_channel,
    7
  );

  headwater_state_update_samples_per_beat(
    &dummy_state.multiplier_b_channel,
    7
  );

  int8_t expected_outputs[9][3] = {
    {1, 1, 0},
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0},
    {0, 0, 0},
    {1, 1, 0},
    {0, 0, 0}
  };

  uint8_t message[50];
  for(uint8_t i = 0; i < 9; i++) {
    headwater_state_cycle(&dummy_state);

    sprintf(message, "Bad bpm output for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      expected_outputs[i][0],
      dummy_state.bpm_channel.output,
      message
    );

    sprintf(message, "Bad multiplier a output for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      expected_outputs[i][1],
      dummy_state.multiplier_a_channel.output,
      message
    );

    sprintf(message, "Bad multiplier b output for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      expected_outputs[i][2],
      dummy_state.multiplier_b_channel.output,
      message
    );
  }
}

TEST(headwater_state, test_headwater_state_cycle_external) {
  dummy_state.output_enabled = false;
  dummy_state.samples_per_second = 10;
  dummy_state.samples_since_reset_count = 0;
  dummy_state.mode = HEADWATER_STATE_MODE_EXTERNAL;
  dummy_state.multiplier_a_channel.multiplier = 2;
  dummy_state.multiplier_b_channel.multiplier = 4;

  int16_t initial_bpm = dummy_state.bpm;

  // in | reset count | since reset | bpm | a | b
  int16_t input_outputs[100][7] = {
    {0, 0, 1, 0, 0, 0, initial_bpm}, // 0
    {0, 0, 2, 0, 0, 0, initial_bpm},
    {0, 0, 3, 0, 0, 0, initial_bpm},
    {0, 0, 4, 0, 0, 0, initial_bpm},
    {0, 0, 5, 0, 0, 0, initial_bpm},
    {0, 0, 6, 0, 0, 0, initial_bpm},
    {0, 0, 7, 0, 0, 0, initial_bpm},
    {0, 0, 8, 0, 0, 0, initial_bpm},
    {0, 0, 9, 0, 0, 0, initial_bpm},
    {0, 0, 10, 0, 0, 0, initial_bpm},
    {0, 0, 11, 0, 0, 0, initial_bpm},
    {0, 0, 12, 0, 0, 0, initial_bpm},

    {1, 1, 1, 0, 0, 0, initial_bpm}, // 12
    {0, 1, 2, 0, 0, 0, initial_bpm},
    {0, 1, 3, 0, 0, 0, initial_bpm},
    {0, 1, 4, 0, 0, 0, initial_bpm},
    {0, 1, 5, 0, 0, 0, initial_bpm},
    {0, 1, 6, 0, 0, 0, initial_bpm},
    {0, 1, 7, 0, 0, 0, initial_bpm},
    {0, 1, 8, 0, 0, 0, initial_bpm},

    {1, 2, 1, 1, 1, 1, 750}, // 20
    {0, 2, 2, 0, 0, 0, 750},
    {0, 2, 3, 0, 0, 1, 750},
    {0, 2, 4, 0, 0, 0, 750},
    {0, 2, 5, 0, 1, 1, 750},
    {0, 2, 6, 0, 0, 0, 750},
    {0, 2, 7, 0, 0, 1, 750},
    {0, 2, 8, 0, 0, 0, 750},

    {1, 3, 1, 1, 1, 1, 750}, // 28
    {0, 3, 2, 0, 0, 0, 750},
    {0, 3, 3, 0, 0, 1, 750},
    {0, 3, 4, 0, 0, 0, 750},
    {0, 3, 5, 0, 1, 1, 750},
    {0, 3, 6, 0, 0, 0, 750},
    {0, 3, 7, 0, 0, 1, 750},
    {0, 3, 8, 0, 0, 0, 750},
    {0, 3, 9, 0, 0, 0, 750},
    {0, 3, 10, 0, 0, 0, 750},
    {0, 3, 11, 0, 0, 0, 750},
    {0, 3, 12, 0, 0, 0, 750},
    {0, 3, 13, 0, 0, 0, 750},
    {0, 3, 14, 0, 0, 0, 750},
    {0, 3, 15, 0, 0, 0, 750},
    {0, 3, 16, 0, 0, 0, 750},

    {1, 4, 1, 1, 1, 1, 375}, // 44
    {0, 4, 2, 0, 0, 0, 375},
    {0, 4, 3, 0, 0, 0, 375},
    {0, 4, 4, 0, 0, 0, 375},
    {0, 4, 5, 0, 0, 1, 375},
    {0, 4, 6, 0, 0, 0, 375},
    {0, 4, 7, 0, 0, 0, 375},
    {0, 4, 8, 0, 0, 0, 375},
    {0, 4, 9, 0, 1, 1, 375},
    {0, 4, 10, 0, 0, 0, 375},
    {0, 4, 11, 0, 0, 0, 375},
    {0, 4, 12, 0, 0, 0, 375},
    {0, 4, 13, 0, 0, 1, 375},
    {0, 4, 14, 0, 0, 0, 375},
    {0, 4, 15, 0, 0, 0, 375},
    {0, 4, 16, 0, 0, 0, 375},

    {1, 5, 1, 1, 1, 1, 375}, // 60
    {0, 5, 2, 0, 0, 0, 375},
    {0, 5, 3, 0, 0, 0, 375},
    {0, 5, 4, 0, 0, 0, 375},
    {0, 5, 5, 0, 0, 1, 375},

    {1, 6, 1, 1, 1, 1, 1200}, // 65
    {0, 6, 2, 0, 0, 0, 1200},
    {0, 6, 3, 0, 1, 1, 1200},
    {0, 6, 4, 0, 0, 1, 1200},
    {0, 6, 5, 0, 0, 1, 1200}
  };

  uint8_t message[50];
  for(uint8_t i = 0; i < 70; i++) {

    if(input_outputs[i][0] == 1) {
      headwater_state_play(&dummy_state);
    }

    headwater_state_cycle(&dummy_state);
    headwater_state_handle_change(&dummy_state);

    sprintf(message, "Wrong reset_count for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      input_outputs[i][1],
      dummy_state.reset_count,
      message
    );

    sprintf(message, "Wrong samples_since_reset_count for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      input_outputs[i][2],
      dummy_state.samples_since_reset_count,
      message
    );

    sprintf(message, "Wrong bpm output for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      input_outputs[i][3],
      dummy_state.bpm_channel.output,
      message
    );

    sprintf(message, "Wrong multiplier a output for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      input_outputs[i][4],
      dummy_state.multiplier_a_channel.output,
      message
    );

    sprintf(message, "Wrong multiplier b output for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      input_outputs[i][5],
      dummy_state.multiplier_b_channel.output,
      message
    );

    sprintf(message, "Wrong bpm display for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(
      input_outputs[i][6],
      dummy_state.bpm,
      message
    );
  }

  headwater_state_stop(&dummy_state);

  TEST_ASSERT_EQUAL(0, dummy_state.reset_count);
  TEST_ASSERT_EQUAL(1200, dummy_state.bpm);

  headwater_state_cycle(&dummy_state);

  TEST_ASSERT_EQUAL(0, dummy_state.reset_count);
  TEST_ASSERT_EQUAL(1200, dummy_state.bpm);
}

TEST(headwater_state, test_headwater_state_prevent_int_mode_overflow) {
  headwater_state_play(&dummy_state);

  uint32_t outputs = 0;
  uint16_t previous_bpm_samples = 65535;
  uint16_t previous_multiplier_a_samples = 65535;
  uint16_t previous_multiplier_b_samples = 65535;
  for(uint32_t i = 0; i < 100000; i++) {
    headwater_state_cycle(&dummy_state);
    outputs += dummy_state.bpm_channel.output;

    // sample count should always change
    TEST_ASSERT_NOT_EQUAL(
      previous_bpm_samples,
      dummy_state.bpm_channel.samples
    );
    previous_bpm_samples = dummy_state.bpm_channel.samples;

    TEST_ASSERT_NOT_EQUAL(
      previous_multiplier_a_samples,
      dummy_state.multiplier_a_channel.samples
    );
    previous_multiplier_a_samples = dummy_state.multiplier_a_channel.samples;

    TEST_ASSERT_NOT_EQUAL(
      previous_multiplier_b_samples,
      dummy_state.multiplier_b_channel.samples
    );
    previous_multiplier_b_samples = dummy_state.multiplier_b_channel.samples;
  }

  TEST_ASSERT_EQUAL(true, dummy_state.output_enabled);
  TEST_ASSERT_EQUAL(100, outputs);

  for(uint32_t i = 0; i < 100000; i++) {
    headwater_state_cycle(&dummy_state);
    outputs += dummy_state.bpm_channel.output;
  }

  TEST_ASSERT_EQUAL(200, outputs);
}

TEST(
  headwater_state,
  test_headwater_state_update_samples_per_beat_with_remainder
) {
  uint16_t samples_per_beat = 2003;
  uint8_t multiplier = 5;

  HeadwaterStateChannel channel =
    headwater_state_channel_new(samples_per_beat);

  channel.multiplier = multiplier;

  headwater_state_update_samples_per_beat(
    &channel,
    samples_per_beat
  );

  TEST_ASSERT_EQUAL(166, channel.remainder_frequency);
  TEST_ASSERT_EQUAL(166, channel.remainder_priority);
  TEST_ASSERT_EQUAL(250, channel.no_remainder_frequency);
  TEST_ASSERT_EQUAL(250, channel.no_remainder_priority);
}

TEST(headwater_state, test_headwater_state_evenly_distributed_groups) {
  typedef struct TestCase {
    uint16_t samples_per_beat;
    uint8_t multiplier;
    uint8_t expected[100];
  } TestCase;

  TestCase test_cases[100] = {
    {6, 3, {2, 2, 2}}, // 0
    {7, 3, {2, 2, 3}},
    {8, 3, {3, 2, 3}},

    {8, 4, {2, 2, 2, 2}}, // 3
    {9, 4, {2, 2, 2, 3}},
    {10, 4, {2, 3, 2, 3}},
    {11, 4, {3, 3, 3, 2}},

    {10, 5, {2, 2, 2, 2, 2}}, // 7
    {11, 5, {2, 2, 2, 2, 3}},
    {12, 5, {2, 3, 2, 2, 3}},
    {13, 5, {3, 2, 3, 3, 2}},
    {14, 5, {3, 3, 3, 2, 3}},

    {12, 6, {2, 2, 2, 2, 2, 2}}, // 12
    {13, 6, {2, 2, 2, 2, 2, 3}},
    {14, 6, {2, 2, 3, 2, 2, 3}},
    {15, 6, {2, 3, 2, 3, 2, 3}},
    {16, 6, {3, 2, 3, 3, 2, 3}},
    {17, 6, {3, 3, 3, 3, 2, 3}},

    {14, 7, {2, 2, 2, 2, 2, 2, 2}}, // 18
    {15, 7, {2, 2, 2, 2, 2, 2, 3}},
    {16, 7, {2, 2, 3, 2, 2, 2, 3}},
    {17, 7, {2, 3, 2, 3, 2, 3, 2}},
    {18, 7, {3, 2, 3, 2, 3, 2, 3}},
    {19, 7, {3, 3, 2, 3, 3, 2, 3}},
    {20, 7, {3, 3, 3, 3, 3, 3, 2}},

    // 25
    {42, 21, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}},
    {43, 21, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3}},
    {44, 21, {2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3}},
    {45, 21, {2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 3}},
    {46, 21, {2, 2, 2, 2, 3, 2, 2, 2, 2, 3, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 3}},
    {47, 21, {2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 2, 3}},
    {48, 21, {2, 2, 3, 2, 2, 2, 3, 2, 2, 3, 2, 2, 2, 3, 2, 2, 3, 2, 2, 2, 3}}
  };

  for(uint8_t i = 0; i < 32; i++) {
    TestCase test_case = test_cases[i];

    HeadwaterStateChannel channel = headwater_state_channel_new(
      test_case.samples_per_beat
    );

    channel.multiplier = test_case.multiplier;
    headwater_state_update_samples_per_beat(
      &channel,
      test_case.samples_per_beat
    );

    // for samples_per_beat...
    uint8_t message[50];
    for(uint8_t n = 0; n < test_case.multiplier; n++) {
      headwater_state_channel_fire(&channel);

      sprintf(message, "Bad result for case %i, beat %i", i, n);

      TEST_ASSERT_EQUAL_MESSAGE(
        test_case.expected[n],
        channel.samples_per_beat_with_remainder,
        message
      );
    }
  }
}

// TODO test that samples per beat for all channels waits until next beat
// to update after change to bpm or multiplier (preset changes too)

TEST_GROUP_RUNNER(headwater_state) {
  RUN_TEST_CASE(headwater_state, test_headwater_state_samples_to_bpm);
  RUN_TEST_CASE(headwater_state, test_headwater_state_bpm_to_samples);
  RUN_TEST_CASE(headwater_state, test_headwater_state_update_samples_per_beat);
  RUN_TEST_CASE(
    headwater_state,
    test_headwater_state_update_samples_per_beat_multiplied
  );
  RUN_TEST_CASE(headwater_state, test_headwater_state_increment_counts);
  RUN_TEST_CASE(headwater_state, test_headwater_state_stop);
  RUN_TEST_CASE(headwater_state, test_headwater_state_cycle_internal);
  RUN_TEST_CASE(headwater_state, test_headwater_state_cycle_external);
  RUN_TEST_CASE(
    headwater_state,
    test_headwater_state_prevent_int_mode_overflow
  );
  RUN_TEST_CASE(
    headwater_state,
    test_headwater_state_update_samples_per_beat_with_remainder
  );
  RUN_TEST_CASE(
    headwater_state,
    test_headwater_state_evenly_distributed_groups
  );
}
