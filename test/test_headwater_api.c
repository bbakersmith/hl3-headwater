#include "unity.h"
#include "unity_fixture.h"

#include "bytes.h"
#include "headwater_api.h"
#include "headwater_state.h"

API dummy_api;
APIRequest dummy_request;
HeadwaterState dummy_state;
uint8_t dummy_serial_register;

TEST_GROUP(headwater_api);

TEST_SETUP(headwater_api) {
  dummy_request = api_new_request();

  // TODO creation function?
  HeadwaterState dummy_state_;
  dummy_state = dummy_state_;

  API dummy_api_ = {
    .payload_preprocessor = &headwater_api_payload_preprocessor,
    .payload_postprocessor = &headwater_api_payload_postprocessor,
    .request = &dummy_request,
    .serial_register = &dummy_serial_register,
    .state = &dummy_state
  };
  dummy_api = dummy_api_;
};

TEST_TEAR_DOWN(headwater_api) {};

// GET Tests

void assert_headwater_api_get(
  API *api,
  HEADWATER_API_CMD command,
  uint8_t *target,
  uint8_t value
) {
  uint8_t result;
  *target = value;

  dummy_serial_register = (1 << API_HEADER_SIZE1) | command;

  api_handle_interrupt(api);

  TEST_ASSERT_EQUAL(value, dummy_serial_register);
}

void assert_headwater_api_get_16bit(
  API *api,
  HEADWATER_API_CMD command,
  uint16_t *target,
  uint16_t value
) {
  uint8_t high;
  uint8_t low;
  *target = value;

  dummy_serial_register = (1 << API_HEADER_SIZE1) | command;

  api_handle_interrupt(api);
  high = dummy_serial_register;

  api_handle_interrupt(api);
  low = dummy_serial_register;

  TEST_ASSERT_EQUAL(value, bytes_high_low_to_16bit(high, low));
}

TEST(headwater_api, test_api_cmd_get_bpm) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_BPM,
    &dummy_state.bpm,
    1357
  );
}

TEST(headwater_api, test_api_cmd_get_multiplier_a) {
  assert_headwater_api_get(
    &dummy_api,
    HEADWATER_API_GET_MULTIPLIER_A,
    &dummy_state.multiplier_a,
    5
  );
}

TEST(headwater_api, test_api_cmd_get_multiplier_b) {
  assert_headwater_api_get(
    &dummy_api,
    HEADWATER_API_GET_MULTIPLIER_B,
    &dummy_state.multiplier_b,
    97
  );
}

TEST(headwater_api, test_api_cmd_get_samples_per_bpm) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLES_PER_BPM,
    &dummy_state.samples_per_bpm,
    1122
  );
}

TEST(headwater_api, test_api_cmd_get_samples_per_multiplier_a) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_A,
    &dummy_state.samples_per_multiplier_a,
    1
  );
}

TEST(headwater_api, test_api_cmd_get_samples_per_multiplier_b) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLES_PER_MULTIPLIER_B,
    &dummy_state.samples_per_multiplier_b,
    60000
  );
}

TEST(headwater_api, test_api_cmd_get_count_multiplier_a) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_COUNT_MULTIPLIER_A,
    &dummy_state.count_multiplier_a,
    133
  );
}

TEST(headwater_api, test_api_cmd_get_count_multiplier_b) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_COUNT_MULTIPLIER_B,
    &dummy_state.count_multiplier_b,
    31000
  );
}

TEST(headwater_api, test_api_cmd_get_sample_count_bpm) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLE_COUNT_BPM,
    &dummy_state.sample_count_bpm,
    2345
  );
}

TEST(headwater_api, test_api_cmd_get_sample_count_multiplier_a) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLE_COUNT_MULTIPLIER_A,
    &dummy_state.sample_count_multiplier_a,
    10000
  );
}

TEST(headwater_api, test_api_cmd_get_sample_count_multiplier_b) {
  assert_headwater_api_get_16bit(
    &dummy_api,
    HEADWATER_API_GET_SAMPLE_COUNT_MULTIPLIER_B,
    &dummy_state.sample_count_multiplier_b,
    13
  );
}

TEST(headwater_api, test_api_cmd_get_cv_bpm) {
  assert_headwater_api_get(
    &dummy_api,
    HEADWATER_API_GET_CV_BPM,
    &dummy_state.cv_bpm,
    1
  );
}

TEST(headwater_api, test_api_cmd_get_cv_bpm0) {
  assert_headwater_api_get(
    &dummy_api,
    HEADWATER_API_GET_CV_BPM,
    &dummy_state.cv_bpm,
    0
  );
}

TEST(headwater_api, test_api_cmd_get_cv_multiplier_a) {
  assert_headwater_api_get(
    &dummy_api,
    HEADWATER_API_GET_CV_MULTIPLIER_A,
    &dummy_state.cv_multiplier_a,
    10
  );
}

TEST(headwater_api, test_api_cmd_get_cv_multiplier_b) {
  assert_headwater_api_get(
    &dummy_api,
    HEADWATER_API_GET_CV_MULTIPLIER_B,
    &dummy_state.cv_multiplier_b,
    255
  );
}

// UPDATE Tests

// TODO remove?
void assert_headwater_api_update(
  API *api,
  HEADWATER_API_CMD command,
  uint8_t *target,
  uint8_t value,
  uint8_t change_flags
) {
  dummy_serial_register = command;

  api_handle_interrupt(api);

  TEST_ASSERT_EQUAL(value, *target);
  TEST_ASSERT_EQUAL(change_flags, api->state->change_flags);
}

void assert_headwater_api_update_8bit(
  API *api,
  HEADWATER_API_CMD command,
  uint8_t *target,
  uint8_t value,
  uint8_t change_flags
) {
  dummy_serial_register = (1 << API_HEADER_SIZE0) | command;

  api_handle_interrupt(api);

  dummy_serial_register = value;

  api_handle_interrupt(api);

  TEST_ASSERT_EQUAL(value, *target);
  TEST_ASSERT_EQUAL(change_flags, api->state->change_flags);
}

void assert_headwater_api_update_16bit(
  API *api,
  HEADWATER_API_CMD command,
  uint16_t *target,
  uint16_t value,
  uint8_t change_flags
) {
  uint8_t high = bytes_16bit_to_high(value);
  uint8_t low = bytes_16bit_to_low(value);

  dummy_serial_register = (1 << API_HEADER_SIZE1) | command;

  api_handle_interrupt(api);

  dummy_serial_register = high;

  api_handle_interrupt(api);

  dummy_serial_register = low;

  api_handle_interrupt(api);

  TEST_ASSERT_EQUAL(value, *target);
  TEST_ASSERT_EQUAL(change_flags, api->state->change_flags);
}

TEST(headwater_api, test_api_cmd_update_bpm) {
  assert_headwater_api_update_16bit(
    &dummy_api,
    HEADWATER_API_UPDATE_BPM,
    &dummy_state.bpm,
    543,
    (1 << HEADWATER_STATE_CHANGE_BPM)
  );
}

TEST(headwater_api, test_api_cmd_update_multiplier_a) {
  assert_headwater_api_update_8bit(
    &dummy_api,
    HEADWATER_API_UPDATE_MULTIPLIER_A,
    &dummy_state.multiplier_a,
    123,
    (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_A)
  );
}

TEST(headwater_api, test_api_cmd_update_multiplier_b) {
  assert_headwater_api_update_8bit(
    &dummy_api,
    HEADWATER_API_UPDATE_MULTIPLIER_B,
    &dummy_state.multiplier_b,
    255,
    (1 << HEADWATER_STATE_CHANGE_MULTIPLIER_B)
  );
}

TEST(headwater_api, test_api_cmd_update_play) {
  dummy_serial_register = HEADWATER_API_UPDATE_PLAY;

  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL(
    (1 << HEADWATER_STATE_CHANGE_PLAY),
    dummy_api.state->change_flags
  );
}

TEST(headwater_api, test_api_cmd_update_reset) {
  dummy_serial_register = HEADWATER_API_UPDATE_RESET;

  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL(
    (1 << HEADWATER_STATE_CHANGE_RESET),
    dummy_api.state->change_flags
  );
}

TEST(headwater_api, test_api_cmd_update_stop) {
  dummy_serial_register = HEADWATER_API_UPDATE_STOP;

  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL(
    (1 << HEADWATER_STATE_CHANGE_STOP),
    dummy_api.state->change_flags
  );
}

TEST(headwater_api, test_api_cmd_update_mode) {
  assert_headwater_api_update_8bit(
    &dummy_api,
    HEADWATER_API_UPDATE_MODE,
    &dummy_state.mode,
    3,
    (1 << HEADWATER_STATE_CHANGE_MODE)
  );
}

TEST_GROUP_RUNNER(headwater_api) {
  // GET
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
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_cv_bpm);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_cv_bpm);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_cv_multiplier_a);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_cv_multiplier_b);

  // UPDATE
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_bpm);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_multiplier_a);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_multiplier_b);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_play);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_reset);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_stop);
  RUN_TEST_CASE(headwater_api, test_api_cmd_update_mode);
}
