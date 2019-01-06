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

TEST(headwater_api, test_api_cmd_get_bpm) {
  uint8_t high;
  uint8_t low;
  uint16_t dummy_tbpm = 1234;
  dummy_state.tbpm = dummy_tbpm;

  dummy_serial_register = (1 << API_HEADER_SIZE1) | HEADWATER_API_GET_BPM;

  api_handle_interrupt(&dummy_api);
  high = dummy_serial_register;

  api_handle_interrupt(&dummy_api);
  low = dummy_serial_register;

  TEST_ASSERT_EQUAL(dummy_tbpm, dummy_state.tbpm);
  TEST_ASSERT_EQUAL(dummy_tbpm, bytes_high_low_to_16bit(high, low));
}

TEST(headwater_api, test_api_cmd_get_multiplier_a) {
  uint8_t result;
  uint8_t dummy_multiplier_a = 12;
  dummy_state.multiplier_a = dummy_multiplier_a;

  dummy_serial_register = (1 << API_HEADER_SIZE0) | HEADWATER_API_GET_MULTIPLIER_A;

  api_handle_interrupt(&dummy_api);
  result = dummy_serial_register;

  TEST_ASSERT_EQUAL(dummy_multiplier_a, dummy_state.multiplier_a);
  TEST_ASSERT_EQUAL(dummy_multiplier_a, result);
}

TEST(headwater_api, test_api_cmd_get_multiplier_b) {
  uint8_t result;
  uint8_t dummy_multiplier_b = 5;
  dummy_state.multiplier_b = dummy_multiplier_b;

  dummy_serial_register = (1 << API_HEADER_SIZE0) | HEADWATER_API_GET_MULTIPLIER_B;

  api_handle_interrupt(&dummy_api);
  result = dummy_serial_register;

  TEST_ASSERT_EQUAL(dummy_multiplier_b, dummy_state.multiplier_b);
  TEST_ASSERT_EQUAL(dummy_multiplier_b, result);
}

TEST_GROUP_RUNNER(headwater_api) {
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_bpm);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_multiplier_a);
  RUN_TEST_CASE(headwater_api, test_api_cmd_get_multiplier_b);
}
