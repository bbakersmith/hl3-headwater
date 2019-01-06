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

  dummy_request.index = 1;
  dummy_request.size = 2;

  dummy_request.payload[1] = outgoing_value;
  dummy_serial_register = incoming_value;

  api_handle_request(&dummy_request, &dummy_serial_register);

  TEST_ASSERT_EQUAL(outgoing_value, dummy_serial_register);
  TEST_ASSERT_EQUAL(incoming_value, dummy_request.payload[0]);
  TEST_ASSERT_EQUAL(2, dummy_request.index);
}

TEST(api, test_api_handle_request_size2) {
  uint8_t incoming_value1 = 12;
  uint8_t outgoing_value1 = 34;

  uint8_t incoming_value2 = 56;
  uint8_t outgoing_value2 = 78;

  dummy_request.index = 1;
  dummy_request.size = 3;
  dummy_request.payload[1] = outgoing_value1;
  dummy_request.payload[2] = outgoing_value2;

  dummy_serial_register = incoming_value1;

  api_handle_request(&dummy_request, &dummy_serial_register);

  TEST_ASSERT_EQUAL(outgoing_value1, dummy_serial_register);
  TEST_ASSERT_EQUAL(incoming_value1, dummy_request.payload[0]);
  TEST_ASSERT_EQUAL(2, dummy_request.index);

  dummy_serial_register = incoming_value2;

  api_handle_request(&dummy_request, &dummy_serial_register);

  TEST_ASSERT_EQUAL(outgoing_value2, dummy_serial_register);
  TEST_ASSERT_EQUAL(incoming_value2, dummy_request.payload[1]);
  TEST_ASSERT_EQUAL(3, dummy_request.index);
}

void dummy_payload_preprocessor(API *api) {
  uint8_t dummy_payload[8] = {3, 2, 1};
  api_new_payload(api->request, dummy_payload);
}

TEST(api, test_api_payload_preprocessor) {
  uint8_t dummy_size = 3;
  dummy_request.size = dummy_size;
  API dummy_api = {
    .request = &dummy_request,
    .payload_preprocessor = &dummy_payload_preprocessor,
    .serial_register = &dummy_serial_register,
  };

  api_payload_preprocessor(&dummy_api);

  uint8_t expected_payload[8] = {3, 2, 1};
  assert_payload_equal(expected_payload, dummy_api.request->payload, dummy_size);
  TEST_ASSERT_EQUAL(0, dummy_request.index);
}

void dummy_payload_preprocessor_full(API *api) {
  if(api->request->command == 1) {
    uint8_t dummy_payload[8] = {14, 15, 16};
    api_new_payload(api->request, dummy_payload);
  }
}

uint8_t dummy_payload_postprocessor_result1[2] = {0, 0};
uint8_t dummy_payload_postprocessor_result2 = 0;

void dummy_payload_postprocessor_full(API *api) {
  switch(api->request->command) {
    case 1:
      dummy_payload_postprocessor_result1[0] = api->request->payload[0];
      dummy_payload_postprocessor_result1[1] = api->request->payload[1];
      break;
    case 2:
      dummy_payload_postprocessor_result2 = api->request->payload[0];
      break;
  }
}

TEST(api, test_api_handle_interrupt) {
  API dummy_api = {
    .request = &dummy_request,
    .payload_preprocessor = &dummy_payload_preprocessor_full,
    .payload_postprocessor = &dummy_payload_postprocessor_full,
    .serial_register = &dummy_serial_register
  };

  // A new
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "A index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "A size");

  // B header size 2
  dummy_serial_register = (1 << API_HEADER_SIZE1) | (1 << API_HEADER_CMD0);
  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL_MESSAGE(1, dummy_api.request->index, "B index");
  TEST_ASSERT_EQUAL_MESSAGE(2, dummy_api.request->size, "B size");
  TEST_ASSERT_EQUAL_MESSAGE(14, dummy_serial_register, "B serial_register");

  // C payload 1
  dummy_serial_register = 11;
  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL_MESSAGE(2, dummy_api.request->index, "C index");
  TEST_ASSERT_EQUAL_MESSAGE(2, dummy_api.request->size, "C size");
  TEST_ASSERT_EQUAL_MESSAGE(15, dummy_serial_register, "C serial_register");

  // D payload 2
  dummy_serial_register = 22;
  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "D index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "D size");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_serial_register, "D serial_register");
  TEST_ASSERT_EQUAL_MESSAGE(
    11,
    dummy_payload_postprocessor_result1[0],
    "D post0"
  );
  TEST_ASSERT_EQUAL_MESSAGE(
    22,
    dummy_payload_postprocessor_result1[1],
    "D post0"
  );
  TEST_ASSERT_EQUAL_MESSAGE(
    API_CMD_NEW_REQUEST,
    dummy_api.request->command,
    "D command"
  );

  // E header size 0
  dummy_serial_register = (1 << API_HEADER_CMD3);
  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "E index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "E size");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_serial_register, "E serial_register");
  TEST_ASSERT_EQUAL_MESSAGE(
    API_CMD_NEW_REQUEST,
    dummy_api.request->command,
    "E command"
  );

  // F header size 1
  dummy_serial_register = (1 << API_HEADER_SIZE0) | (1 << API_HEADER_CMD1);
  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL_MESSAGE(1, dummy_api.request->index, "F index");
  TEST_ASSERT_EQUAL_MESSAGE(1, dummy_api.request->size, "F size");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_serial_register, "F serial_register");

  // G payload 1
  dummy_serial_register = 77;
  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "G index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "G size");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_serial_register, "G serial_register");
  TEST_ASSERT_EQUAL_MESSAGE(
    77,
    dummy_payload_postprocessor_result2,
    "G post"
  );
  TEST_ASSERT_EQUAL_MESSAGE(
    API_CMD_NEW_REQUEST,
    dummy_api.request->command,
    "G command"
  );

  // H header size 0
  dummy_serial_register = (1 << API_HEADER_CMD3);
  api_handle_interrupt(&dummy_api);

  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "H index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "H size");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_serial_register, "H serial_register");
  TEST_ASSERT_EQUAL_MESSAGE(
    API_CMD_NEW_REQUEST,
    dummy_api.request->command,
    "H command"
  );
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
  RUN_TEST_CASE(api, test_api_payload_preprocessor);
  RUN_TEST_CASE(api, test_api_handle_interrupt);
}
