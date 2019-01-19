#include "unity.h"
#include "unity_fixture.h"

#include "bytes.h"
#include "headwater_api.h"
#include "headwater_state.h"

API dummy_api;

TEST_GROUP(headwater_api);

TEST_SETUP(headwater_api) {
  APIRequest dummy_request = api_new_request();
  HeadwaterState dummy_state = headwater_state_new();

  API dummy_api_ = {
    .payload_preprocessor = &headwater_api_payload_preprocessor,
    .payload_postprocessor = &headwater_api_payload_postprocessor,
    .request = dummy_request,
    .state = dummy_state
  };
  dummy_api = dummy_api_;
};

TEST_TEAR_DOWN(headwater_api) {};

// GET Tests

void assert_headwater_api_get_8bit(
  API *api,
  HEADWATER_API_CMD command,
  volatile uint8_t *target,
  uint8_t value
) {
  uint8_t result;
  *target = value;
  uint8_t header = (1 << API_HEADER_SIZE0) | command;

  uint8_t outgoing = api_handle_interrupt(api, header);

  TEST_ASSERT_EQUAL(value, outgoing);
}

void assert_headwater_api_get_16bit(
  API *api,
  HEADWATER_API_CMD command,
  volatile uint16_t *target,
  uint16_t value
) {
  uint8_t header = (1 << API_HEADER_SIZE1) | command;
  *target = value;

  uint8_t high = api_handle_interrupt(api, header);
  uint8_t low = api_handle_interrupt(api, 0);

  TEST_ASSERT_EQUAL(value, bytes_high_low_to_16bit(high, low));
}

TEST(headwater_api, test_api_cmd_get_output_enabled) {
  assert_headwater_api_get_8bit(
    &dummy_api,
    HEADWATER_API_GET_OUTPUT_ENABLED,
    &dummy_api.state.output_enabled,
    1
  );
}

TEST(headwater_api, test_api_cmd_get_mode) {
  assert_headwater_api_get_8bit(
    &dummy_api,
    HEADWATER_API_GET_MODE,
    &dummy_api.state.mode,
    3
  );
}

TEST(headwater_api, test_api_cmd_get_bpm) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_BPM,
    &dummy_api.state.bpm,
    1357
  );
}

TEST(headwater_api, test_api_cmd_get_multiplier_a) {
  assert_headwater_api_get_8bit(
    &dummy_api,
    HEADWATER_API_GET_MULTIPLIER_A,
    &dummy_api.state.multiplier_a_channel.multiplier,
    5
  );
}

TEST(headwater_api, test_api_cmd_get_multiplier_b) {
  assert_headwater_api_get_8bit(
    &dummy_api,
    HEADWATER_API_GET_MULTIPLIER_B,
    &dummy_api.state.multiplier_b_channel.multiplier,
    97
  );
}

TEST(headwater_api, test_api_cmd_get_samples_per_bpm) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLES_PER_BPM,
    &dummy_api.state.bpm_channel.samples_per_beat,
    1122
  );
}

TEST(headwater_api, test_api_cmd_get_samples_per_multiplier_a) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_A,
    &dummy_api.state.multiplier_a_channel.samples_per_beat,
    1
  );
}

TEST(headwater_api, test_api_cmd_get_samples_per_multiplier_b) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_B,
    &dummy_api.state.multiplier_b_channel.samples_per_beat,
    60000
  );
}

TEST(headwater_api, test_api_cmd_get_count_multiplier_a) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_COUNT_MULTIPLIER_A,
    &dummy_api.state.multiplier_a_channel.beats,
    133
  );
}

TEST(headwater_api, test_api_cmd_get_count_multiplier_b) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_COUNT_MULTIPLIER_B,
    &dummy_api.state.multiplier_b_channel.beats,
    31000
  );
}

TEST(headwater_api, test_api_cmd_get_sample_count_bpm) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLE_COUNT_BPM,
    &dummy_api.state.bpm_channel.samples,
    2345
  );
}

TEST(headwater_api, test_api_cmd_get_sample_count_multiplier_a) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLE_COUNT_MULTIPLIER_A,
    &dummy_api.state.multiplier_a_channel.samples,
    10000
  );
}

TEST(headwater_api, test_api_cmd_get_sample_count_multiplier_b) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLE_COUNT_MULTIPLIER_B,
    &dummy_api.state.multiplier_b_channel.samples,
    13
  );
}

// UPDATE Tests

void assert_headwater_api_update_8bit(
  API *api,
  HEADWATER_API_CMD command,
  volatile uint8_t *target,
  uint8_t value,
  uint8_t change_flags
) {
  uint8_t header = (1 << API_HEADER_SIZE0) | command;

  api_handle_interrupt(api, header);
  api_handle_interrupt(api, value);

  TEST_ASSERT_EQUAL(value, *target);
  TEST_ASSERT_EQUAL(change_flags, api->state.change_flags);
}

void assert_headwater_api_update_16bit(
  API *api,
  HEADWATER_API_CMD command,
  volatile uint16_t *target,
  uint16_t value,
  uint8_t change_flags
) {
  uint8_t header = (1 << API_HEADER_SIZE1) | command;
  uint8_t high = bytes_16bit_to_high(value);
  uint8_t low = bytes_16bit_to_low(value);

  api_handle_interrupt(api, header);
  api_handle_interrupt(api, high);
  api_handle_interrupt(api, low);

  TEST_ASSERT_EQUAL(value, *target);
  TEST_ASSERT_EQUAL(change_flags, api->state.change_flags);
}

TEST(headwater_api, test_api_cmd_update_bpm) {
  assert_headwater_api_update_16bit(
    &dummy_api,
    HEADWATER_API_UPDATE_BPM,
    &dummy_api.state.bpm,
    543,
    (1 << HEADWATER_STATE_CHANGE_BPM)
  );
}

TEST(headwater_api, test_api_cmd_update_multiplier_a) {
  assert_headwater_api_update_8bit(
    &dummy_api,
    HEADWATER_API_UPDATE_MULTIPLIER_A,
    &dummy_api.state.multiplier_a_channel.multiplier,
    123,
    (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A)
  );
}

TEST(headwater_api, test_api_cmd_update_multiplier_b) {
  assert_headwater_api_update_8bit(
    &dummy_api,
    HEADWATER_API_UPDATE_MULTIPLIER_B,
    &dummy_api.state.multiplier_b_channel.multiplier,
    255,
    (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B)
  );
}

TEST(headwater_api, test_api_cmd_update_play) {
  api_handle_interrupt(&dummy_api, HEADWATER_API_UPDATE_PLAY);

  TEST_ASSERT_EQUAL(
    (1 << HEADWATER_STATE_CHANGE_PLAY),
    dummy_api.state.change_flags
  );
}

TEST(headwater_api, test_api_cmd_update_reset) {
  api_handle_interrupt(&dummy_api, HEADWATER_API_UPDATE_RESET);

  TEST_ASSERT_EQUAL(
    (1 << HEADWATER_STATE_CHANGE_RESET),
    dummy_api.state.change_flags
  );
}

TEST(headwater_api, test_api_cmd_update_stop) {
  api_handle_interrupt(&dummy_api, HEADWATER_API_UPDATE_STOP);

  TEST_ASSERT_EQUAL(
    (1 << HEADWATER_STATE_CHANGE_STOP),
    dummy_api.state.change_flags
  );
}

TEST(headwater_api, test_api_cmd_update_mode) {
  assert_headwater_api_update_8bit(
    &dummy_api,
    HEADWATER_API_UPDATE_MODE,
    &dummy_api.state.mode,
    3,
    (1 << HEADWATER_STATE_CHANGE_MODE)
  );
}

TEST_GROUP_RUNNER(headwater_api) {
  // GET
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_output_enabled);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_mode);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_bpm);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_multiplier_a);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_multiplier_b);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_samples_per_bpm);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_samples_per_multiplier_a);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_samples_per_multiplier_b);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_count_multiplier_a);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_count_multiplier_b);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_sample_count_bpm);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_sample_count_multiplier_a);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_sample_count_multiplier_b);

  // UPDATE
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_bpm);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_multiplier_a);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_multiplier_b);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_play);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_reset);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_stop);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_mode);
}
