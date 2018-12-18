#include "headwater.h"
#include "unity.h"
#include "unity_fixture.h"


TEST_GROUP(headwater);


ClockState dummy_clock_state;
uint8_t dummy_output;
uint8_t dummy_multiplied;


void dummy_update_output(uint8_t enabled) {
  dummy_output = enabled;
}


void dummy_update_multiplied(uint8_t enabled) {
  dummy_multiplied = enabled;
}


TEST_SETUP(headwater) {
  dummy_clock_state = create_clock_state();
  dummy_clock_state.running = 1;
  dummy_output = 0;
  dummy_multiplied = 0;
}


TEST_TEAR_DOWN(headwater) {}


TEST(headwater, test_rotary_encoder_modifier) {
  TEST_ASSERT_EQUAL(-1, rotary_encoder_modifier(1));
  TEST_ASSERT_EQUAL(1, rotary_encoder_modifier(0));
}


TEST(headwater, test_tbpm_encoder_modifier) {
  TEST_ASSERT_EQUAL(-1, tbpm_encoder_modifier(1, 1));
  TEST_ASSERT_EQUAL(1, tbpm_encoder_modifier(0, 1));
  TEST_ASSERT_EQUAL(-10, tbpm_encoder_modifier(1, 0));
  TEST_ASSERT_EQUAL(10, tbpm_encoder_modifier(0, 0));
}


TEST(headwater, test_modify_tbpm) {
  TEST_ASSERT_EQUAL(599, modify_tbpm(600, 1, 1));
  TEST_ASSERT_EQUAL(590, modify_tbpm(600, 1, 0));
  TEST_ASSERT_EQUAL(601, modify_tbpm(600, 0, 1));
  TEST_ASSERT_EQUAL(610, modify_tbpm(600, 0, 0));
  TEST_ASSERT_EQUAL(3000, modify_tbpm(3000, 0, 1));
  TEST_ASSERT_EQUAL(3000, modify_tbpm(2995, 0, 0));
  TEST_ASSERT_EQUAL(10, modify_tbpm(1, 1, 1));
  TEST_ASSERT_EQUAL(10, modify_tbpm(5, 1, 0));
}


TEST(headwater, test_modify_multiplier) {
  TEST_ASSERT_EQUAL(11, modify_multiplier(10, 0));
  TEST_ASSERT_EQUAL(9, modify_multiplier(10, 1));
  TEST_ASSERT_EQUAL(99, modify_multiplier(99, 0));
  TEST_ASSERT_EQUAL(1, modify_multiplier(1, 1));
}


TEST(headwater, test_samples_per_output) {
  TEST_ASSERT_EQUAL(1000, samples_per_output(600));
  TEST_ASSERT_EQUAL(500, samples_per_output(1200));
  TEST_ASSERT_EQUAL(250, samples_per_output(2400));
  TEST_ASSERT_EQUAL(200, samples_per_output(3000));
  TEST_ASSERT_EQUAL(497, samples_per_output(1205));
  TEST_ASSERT_EQUAL(60000, samples_per_output(10));
}


TEST(headwater, test_samples_per_multiplied) {
  TEST_ASSERT_EQUAL(20, samples_per_multiplied(600, 50));
  TEST_ASSERT_EQUAL(50, samples_per_multiplied(1200, 10));
  TEST_ASSERT_EQUAL(50, samples_per_multiplied(2400, 5));
  TEST_ASSERT_EQUAL(2, samples_per_multiplied(3000, 100));
  TEST_ASSERT_EQUAL(49, samples_per_multiplied(1205, 10));
  TEST_ASSERT_EQUAL(60000, samples_per_multiplied(10, 1));
}


TEST(headwater, test_update_clock_config) {
  int16_t dummy_tbpm = 3000;
  int8_t dummy_multiplier = 5;
  uint16_t dummy_sample_count = 32;
  uint16_t dummy_multiplied_count = 21;

  dummy_clock_state.sample_count = dummy_sample_count;
  dummy_clock_state.multiplied_count = dummy_multiplied_count;

  update_clock_config(&dummy_clock_state, dummy_tbpm, dummy_multiplier);

  TEST_ASSERT_EQUAL(dummy_tbpm, dummy_clock_state.tbpm);
  TEST_ASSERT_EQUAL(dummy_multiplier, dummy_clock_state.multiplier);
  TEST_ASSERT_EQUAL(200, dummy_clock_state.samples_per_output);
  TEST_ASSERT_EQUAL(40, dummy_clock_state.samples_per_multiplied);
  TEST_ASSERT_EQUAL(dummy_sample_count, dummy_clock_state.sample_count);
  TEST_ASSERT_EQUAL(dummy_multiplied_count, dummy_clock_state.multiplied_count);
}


TEST(headwater, test_update_clock_config_2) {
  int16_t dummy_tbpm = 600;
  int8_t dummy_multiplier = 4;

  update_clock_config(&dummy_clock_state, dummy_tbpm, dummy_multiplier);

  TEST_ASSERT_EQUAL(dummy_tbpm, dummy_clock_state.tbpm);
  TEST_ASSERT_EQUAL(dummy_multiplier, dummy_clock_state.multiplier);
  TEST_ASSERT_EQUAL(1000, dummy_clock_state.samples_per_output);
  TEST_ASSERT_EQUAL(250, dummy_clock_state.samples_per_multiplied);
}


TEST(headwater, test_increment_clock) {
  dummy_clock_state.samples_per_output = 3;
  TEST_ASSERT_EQUAL(0, dummy_clock_state.sample_count);
  increment_clock(&dummy_clock_state);
  TEST_ASSERT_EQUAL(1, dummy_clock_state.sample_count);
  increment_clock(&dummy_clock_state);
  TEST_ASSERT_EQUAL(2, dummy_clock_state.sample_count);
  increment_clock(&dummy_clock_state);
  TEST_ASSERT_EQUAL(0, dummy_clock_state.sample_count);
  increment_clock(&dummy_clock_state);
  TEST_ASSERT_EQUAL(1, dummy_clock_state.sample_count);
}


TEST(headwater, test_cycle_clock) {
  int8_t dummy_multiplier = 3;
  int16_t dummy_samples_per_output = 7;
  int8_t dummy_samples_per_multiplied = 2;

  dummy_clock_state.multiplier = dummy_multiplier;
  dummy_clock_state.samples_per_output = dummy_samples_per_output;
  dummy_clock_state.samples_per_multiplied = dummy_samples_per_multiplied;

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
    cycle_clock(
        &dummy_clock_state,
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


TEST(headwater, test_stop_clock) {
  cycle_clock(
      &dummy_clock_state,
      &dummy_update_output,
      &dummy_update_multiplied
  );

  TEST_ASSERT_EQUAL(1, dummy_output);
  TEST_ASSERT_EQUAL(1, dummy_output);
  TEST_ASSERT_EQUAL(1, dummy_clock_state.sample_count);

  cycle_clock(
      &dummy_clock_state,
      &dummy_update_output,
      &dummy_update_multiplied
  );

  TEST_ASSERT_EQUAL(0, dummy_output);
  TEST_ASSERT_EQUAL(0, dummy_output);
  TEST_ASSERT_EQUAL(2, dummy_clock_state.sample_count);

  stop_clock(&dummy_clock_state);

  for(uint8_t i; i < 10; i++) {
    cycle_clock(
        &dummy_clock_state,
        &dummy_update_output,
        &dummy_update_multiplied
    );

    TEST_ASSERT_EQUAL(0, dummy_output);
    TEST_ASSERT_EQUAL(0, dummy_output);
    TEST_ASSERT_EQUAL(2, dummy_clock_state.sample_count);
  }

  start_clock(&dummy_clock_state);

  cycle_clock(
      &dummy_clock_state,
      &dummy_update_output,
      &dummy_update_multiplied
  );

  TEST_ASSERT_EQUAL(1, dummy_output);
  TEST_ASSERT_EQUAL(1, dummy_output);
  TEST_ASSERT_EQUAL(1, dummy_clock_state.sample_count);
}


TEST_GROUP_RUNNER(headwater) {
  RUN_TEST_CASE(headwater, test_rotary_encoder_modifier);
  RUN_TEST_CASE(headwater, test_tbpm_encoder_modifier);
  RUN_TEST_CASE(headwater, test_modify_tbpm);
  RUN_TEST_CASE(headwater, test_modify_multiplier);
  RUN_TEST_CASE(headwater, test_samples_per_output);
  RUN_TEST_CASE(headwater, test_update_clock_config);
  RUN_TEST_CASE(headwater, test_update_clock_config_2);
  RUN_TEST_CASE(headwater, test_increment_clock);
  RUN_TEST_CASE(headwater, test_cycle_clock);
  RUN_TEST_CASE(headwater, test_stop_clock);
}
