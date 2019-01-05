#include "api.h"
#include "unity.h"
#include "unity_fixture.h"

APIRequest dummy_request;
uint8_t dummy_serial_register;

void assert_payload_equal(uint8_t expected[8], uint8_t actual[8], uint8_t size) {
  for(uint8_t i = 0; i < 8; i++) {
    TEST_ASSERT_EQUAL(expected[i], actual[i]);
  }
}

TEST_GROUP(api);

TEST_SETUP(api) {
  dummy_request = api_new_request();
  dummy_serial_register = 0;
};

TEST_TEAR_DOWN(api) {};

TEST(api, test_api_parse_header_size0_cmd0) {
  uint8_t dummy_header = 0;

  api_parse_header(&dummy_request, dummy_header);

  TEST_ASSERT_EQUAL(0, dummy_request.size);
  TEST_ASSERT_EQUAL(0, dummy_request.command);
}

TEST(api, test_api_parse_header_size1_cmd10) {
  uint8_t dummy_header =
    (1 << API_HEADER_SIZE0)
    | (1 << API_HEADER_CMD3)
    | (1 << API_HEADER_CMD1);

  api_parse_header(&dummy_request, dummy_header);

  TEST_ASSERT_EQUAL(1, dummy_request.size);
  TEST_ASSERT_EQUAL(10, dummy_request.command);
}

TEST(api, test_api_parse_header_size2_cmd21) {
  uint8_t dummy_header =
    (1 << API_HEADER_SIZE1)
    | (1 << API_HEADER_CMD4)
    | (1 << API_HEADER_CMD2)
    | (1 << API_HEADER_CMD0);

  api_parse_header(&dummy_request, dummy_header);

  TEST_ASSERT_EQUAL(2, dummy_request.size);
  TEST_ASSERT_EQUAL(21, dummy_request.command);
}

TEST(api, test_api_parse_header_size7_cmd1) {
  uint8_t dummy_header =
    (1 << API_HEADER_SIZE2)
    | (1 << API_HEADER_SIZE1)
    | (1 << API_HEADER_SIZE0)
    | (1 << API_HEADER_CMD0);

  api_parse_header(&dummy_request, dummy_header);

  TEST_ASSERT_EQUAL(7, dummy_request.size);
  TEST_ASSERT_EQUAL(1, dummy_request.command);
}

TEST(api, test_api_new_request) {
  APIRequest request = api_new_request();

  TEST_ASSERT_EQUAL(API_CMD_NEW_REQUEST, request.command);
  TEST_ASSERT_EQUAL(0, dummy_request.size);
}

TEST(api, test_api_new_payload_size3) {
  uint8_t dummy_payload[8] = {1, 2, 3};
  uint8_t dummy_size = 3;
  dummy_request.size = dummy_size;

  api_new_payload(&dummy_request, dummy_payload);

  assert_payload_equal(dummy_payload, dummy_request.payload, dummy_size);
  TEST_ASSERT_EQUAL(0, dummy_request.index);
}

TEST(api, test_api_new_payload_size8) {
  uint8_t dummy_payload[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  uint8_t dummy_size = 8;
  dummy_request.size = dummy_size;

  api_new_payload(&dummy_request, dummy_payload);

  assert_payload_equal(dummy_payload, dummy_request.payload, dummy_size);
  TEST_ASSERT_EQUAL(0, dummy_request.index);
}

TEST(api, test_api_handle_request_size1) {
  uint8_t incoming_value = 12;
  uint8_t outgoing_value = 34;

  dummy_request.index = 0;
  dummy_request.size = 1;

  dummy_request.payload[0] = outgoing_value;
  dummy_serial_register = incoming_value;

  api_handle_request(&dummy_request, &dummy_serial_register);

  TEST_ASSERT_EQUAL(outgoing_value, dummy_serial_register);
  TEST_ASSERT_EQUAL(incoming_value, dummy_request.payload[0]);
  TEST_ASSERT_EQUAL(1, dummy_request.index);
}

TEST(api, test_api_handle_request_size2) {
  uint8_t incoming_value1 = 12;
  uint8_t outgoing_value1 = 34;

  uint8_t incoming_value2 = 56;
  uint8_t outgoing_value2 = 78;

  dummy_request.index = 0;
  dummy_request.size = 1;
  dummy_request.payload[0] = outgoing_value1;
  dummy_request.payload[1] = outgoing_value2;

  dummy_serial_register = incoming_value1;

  api_handle_request(&dummy_request, &dummy_serial_register);

  TEST_ASSERT_EQUAL(outgoing_value1, dummy_serial_register);
  TEST_ASSERT_EQUAL(incoming_value1, dummy_request.payload[0]);
  TEST_ASSERT_EQUAL(1, dummy_request.index);

  dummy_serial_register = incoming_value2;

  api_handle_request(&dummy_request, &dummy_serial_register);

  TEST_ASSERT_EQUAL(outgoing_value2, dummy_serial_register);
  TEST_ASSERT_EQUAL(incoming_value2, dummy_request.payload[1]);
  TEST_ASSERT_EQUAL(2, dummy_request.index);
}

TEST_GROUP_RUNNER(api) {
  RUN_TEST_CASE(api, test_api_parse_header_size0_cmd0);
  RUN_TEST_CASE(api, test_api_parse_header_size1_cmd10);
  RUN_TEST_CASE(api, test_api_parse_header_size2_cmd21);
  RUN_TEST_CASE(api, test_api_parse_header_size7_cmd1);
  RUN_TEST_CASE(api, test_api_new_request);
  RUN_TEST_CASE(api, test_api_new_payload_size3);
  RUN_TEST_CASE(api, test_api_new_payload_size8);
  RUN_TEST_CASE(api, test_api_handle_request_size1);
  RUN_TEST_CASE(api, test_api_handle_request_size2);
}

/* HeadwaterAPI dummy_api; */
/* HeadwaterAPIRequest dummy_request; */
/* HeadwaterState dummy_state; */
/* uint8_t dummy_serial_register; */
/* uint8_t dummy_payload_get[4]; */
/* uint8_t dummy_payload_update[4]; */
/* uint8_t dummy_payload_empty[4] = {0, 0, 0, 0}; */
/*  */
/* void copy_payload(uint8_t src[4], uint8_t *dest) { */
/*   for(uint8_t i = 0; i < 4; i++) { */
/*     dest[i] = src[i]; */
/*   } */
/* } */
/*  */
/* TEST_GROUP(headwater_api); */
/*  */
/* TEST_SETUP(headwater_api) { */
/*   dummy_serial_register = 0; */
/*   dummy_state = create_headwater_state(); // TODO headwater_state_create */
/*   HeadwaterAPIRequest dummy_request = { */
/*     .command = HEADWATER_API_NEW_COMMAND */
/*   }; */
/*   HeadwaterAPI dummy_api = { */
/*     .request = &dummy_request, */
/*     .serial_register = &dummy_serial_register, */
/*     .state = &dummy_state, */
/*     .get = &dummy_get_handler, */
/*     .update = &dummy_update_handler */
/*   }; */
/* }; */
/*  */
/* TEST_TEAR_DOWN(headwater_api) {}; */
/*  */
/* TEST(headwater_api, test_headwater_api_get) { */
/*   dummy_serial_register = 0x80; */
/*  */
/*   dummy_payload_get[0] = 0x04; */
/*   dummy_payload_get[1] = 0xD2; */
/*  */
/*   TEST_ASSERT_EQUAL(0, dummy_api.request->index); */
/*  */
/*   headwater_api_handle_interrupt(&dummy_api); */
/*   TEST_ASSERT_EQUAL(0x04, dummy_serial_register); */
/*   TEST_ASSERT_EQUAL(1, dummy_api.request->index); */
/*  */
/*   headwater_api_handle_interrupt(&dummy_api); */
/*   TEST_ASSERT_EQUAL(0xD2, dummy_serial_register); */
/*   TEST_ASSERT_EQUAL(HEADWATER_API_NEW_COMMAND, dummy_api.request->command); */
/* } */
/*  */
/* TEST(headwater_api, test_headwater_api_get_samples_per_beat) { */
/*   #<{(| uint16_t dummy_samples_per_beat = 1234; |)}># */
/*   #<{(| dummy_state.samples_per_beat = dummy_samples_per_beat; |)}># */
/*   #<{(|  |)}># */
/*   #<{(| dummy_serial_register = HEADWATER_API_GET_SAMPLES_PER_BEAT; |)}># */
/*   #<{(| headwater_api_handle_interrupt(dummy_api); |)}># */
/*   #<{(| uint8_t high = dummy_serial_register; |)}># */
/*   #<{(|  |)}># */
/*   #<{(| // state change should not affect transfer in progress |)}># */
/*   #<{(| dummy_state.samples_per_beat = 4567; |)}># */
/*   #<{(|  |)}># */
/*   #<{(| headwater_api_handle_interrupt(dummy_api); |)}># */
/*   #<{(| uint8_t low = dummy_serial_register; |)}># */
/*   #<{(|  |)}># */
/*   #<{(| TEST_ASSERT_EQUAL(dummy_samples_per_beat, (high << 8) | low); |)}># */
/* } */
/*  */
/* TEST_GROUP_RUNNER(headwater_api) { */
/*   RUN_TEST_CASE(headwater_api, test_headwater_api_get); */
/* } */
