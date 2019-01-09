#include "api.h"
#include "unity.h"
#include "unity_fixture.h"

APIRequest dummy_request;

void assert_payload_equal(uint8_t expected[8], uint8_t actual[8], uint8_t size) {
  for(uint8_t i = 0; i < 8; i++) {
    TEST_ASSERT_EQUAL(expected[i], actual[i]);
  }
}

TEST_GROUP(api);

TEST_SETUP(api) {
  dummy_request = api_new_request();
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

  uint8_t register_value = api_handle_request(&dummy_request, incoming_value);

  TEST_ASSERT_EQUAL(outgoing_value, register_value);
  TEST_ASSERT_EQUAL(incoming_value, dummy_request.payload[0]);
  TEST_ASSERT_EQUAL(2, dummy_request.index);
}

TEST(api, test_api_handle_request_size2) {
  uint8_t incoming_value1 = 12;
  uint8_t outgoing_value1 = 34;
  uint8_t register_value1;

  uint8_t incoming_value2 = 56;
  uint8_t outgoing_value2 = 78;
  uint8_t register_value2;

  dummy_request.index = 1;
  dummy_request.size = 3;
  dummy_request.payload[1] = outgoing_value1;
  dummy_request.payload[2] = outgoing_value2;

  register_value1 = api_handle_request(&dummy_request, incoming_value1);

  TEST_ASSERT_EQUAL(outgoing_value1, register_value1);
  TEST_ASSERT_EQUAL(incoming_value1, dummy_request.payload[0]);
  TEST_ASSERT_EQUAL(2, dummy_request.index);

  register_value2 = api_handle_request(&dummy_request, incoming_value2);

  TEST_ASSERT_EQUAL(outgoing_value2, register_value2);
  TEST_ASSERT_EQUAL(incoming_value2, dummy_request.payload[1]);
  TEST_ASSERT_EQUAL(3, dummy_request.index);
}

void dummy_payload_preprocessor(API *api) {
  uint8_t dummy_payload[8] = {3, 2, 1};
  api_new_payload(api->request, dummy_payload);
}

void dummy_payload_preprocessor_full(API *api) {
  if(api->request->command == 1) {
    uint8_t dummy_payload[8] = {14, 15, 16};
    api_new_payload(api->request, dummy_payload);
  }
}

uint8_t dummy_payload_postprocessor_result1[2] = {0, 0};
uint8_t dummy_payload_postprocessor_result2 = 0;
uint8_t dummy_payload_postprocessor_result3[8] = {
  0, 0, 0, 0, 0, 0, 0, 0
};

void dummy_payload_postprocessor_full(API *api) {
  switch(api->request->command) {
    case 1:
      dummy_payload_postprocessor_result1[0] = api->request->payload[0];
      dummy_payload_postprocessor_result1[1] = api->request->payload[1];
      break;
    case 2:
      dummy_payload_postprocessor_result2 = api->request->payload[0];
      break;
    case 3:
      dummy_payload_postprocessor_result3[0] = api->request->payload[0];
      dummy_payload_postprocessor_result3[1] = api->request->payload[1];
      dummy_payload_postprocessor_result3[2] = api->request->payload[2];
      dummy_payload_postprocessor_result3[3] = api->request->payload[3];
      dummy_payload_postprocessor_result3[4] = api->request->payload[4];
      dummy_payload_postprocessor_result3[5] = api->request->payload[5];
      dummy_payload_postprocessor_result3[6] = api->request->payload[6];
      dummy_payload_postprocessor_result3[7] = api->request->payload[7];
      break;
  }
}

TEST(api, test_api_handle_interrupt) {
  API dummy_api = {
    .request = &dummy_request,
    .payload_preprocessor = &dummy_payload_preprocessor_full,
    .payload_postprocessor = &dummy_payload_postprocessor_full
  };

  // A new
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "A index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "A size");

  // B header size 2
  uint8_t header_b = (1 << API_HEADER_SIZE1) | (1 << API_HEADER_CMD0);
  uint8_t register_b = api_handle_interrupt(&dummy_api, header_b);

  TEST_ASSERT_EQUAL_MESSAGE(1, dummy_api.request->index, "B index");
  TEST_ASSERT_EQUAL_MESSAGE(2, dummy_api.request->size, "B size");
  TEST_ASSERT_EQUAL_MESSAGE(14, register_b, "B serial_register");

  // C payload 1
  uint8_t register_c = api_handle_interrupt(&dummy_api, 11);

  TEST_ASSERT_EQUAL_MESSAGE(2, dummy_api.request->index, "C index");
  TEST_ASSERT_EQUAL_MESSAGE(2, dummy_api.request->size, "C size");
  TEST_ASSERT_EQUAL_MESSAGE(15, register_c, "C serial_register");

  // D payload 2
  uint8_t register_d = api_handle_interrupt(&dummy_api, 22);

  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "D index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "D size");
  TEST_ASSERT_EQUAL_MESSAGE(0, register_d, "D serial_register");
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
  uint8_t header_e = (1 << API_HEADER_CMD3);
  uint8_t register_e = api_handle_interrupt(&dummy_api, header_e);

  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "E index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "E size");
  TEST_ASSERT_EQUAL_MESSAGE(0, register_e, "E serial_register");
  TEST_ASSERT_EQUAL_MESSAGE(
    API_CMD_NEW_REQUEST,
    dummy_api.request->command,
    "E command"
  );

  // F header size 1
  uint8_t header_f = (1 << API_HEADER_SIZE0) | (1 << API_HEADER_CMD1);
  uint8_t register_f = api_handle_interrupt(&dummy_api, header_f);

  TEST_ASSERT_EQUAL_MESSAGE(1, dummy_api.request->index, "F index");
  TEST_ASSERT_EQUAL_MESSAGE(1, dummy_api.request->size, "F size");
  TEST_ASSERT_EQUAL_MESSAGE(0, register_f, "F serial_register");

  // G payload 1
  uint8_t register_g = api_handle_interrupt(&dummy_api, 77);

  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "G index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "G size");
  TEST_ASSERT_EQUAL_MESSAGE(0, register_g, "G serial_register");
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
  uint8_t header_h = (1 << API_HEADER_CMD3);
  uint8_t register_h = api_handle_interrupt(&dummy_api, header_h);

  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->index, "H index");
  TEST_ASSERT_EQUAL_MESSAGE(0, dummy_api.request->size, "H size");
  TEST_ASSERT_EQUAL_MESSAGE(0, register_h, "H serial_register");
  TEST_ASSERT_EQUAL_MESSAGE(
    API_CMD_NEW_REQUEST,
    dummy_api.request->command,
    "H command"
  );
}

TEST(api, test_api_handle_interrupt_max_payload) {
  API dummy_api = {
    .request = &dummy_request,
    .payload_preprocessor = &dummy_payload_preprocessor_full,
    .payload_postprocessor = &dummy_payload_postprocessor_full
  };

  uint8_t header =
    (1 << API_HEADER_SIZE2)
    | (1 << API_HEADER_SIZE1)
    | (1 << API_HEADER_SIZE0)
    | (1 << API_HEADER_CMD1)
    | (1 << API_HEADER_CMD0);

  api_handle_interrupt(&dummy_api, header);
  api_handle_interrupt(&dummy_api, 9);
  api_handle_interrupt(&dummy_api, 8);
  api_handle_interrupt(&dummy_api, 7);
  api_handle_interrupt(&dummy_api, 6);
  api_handle_interrupt(&dummy_api, 5);
  api_handle_interrupt(&dummy_api, 4);
  api_handle_interrupt(&dummy_api, 3);

  TEST_ASSERT_EQUAL(9, dummy_payload_postprocessor_result3[0]);
  TEST_ASSERT_EQUAL(8, dummy_payload_postprocessor_result3[1]);
  TEST_ASSERT_EQUAL(7, dummy_payload_postprocessor_result3[2]);
  TEST_ASSERT_EQUAL(6, dummy_payload_postprocessor_result3[3]);
  TEST_ASSERT_EQUAL(5, dummy_payload_postprocessor_result3[4]);
  TEST_ASSERT_EQUAL(4, dummy_payload_postprocessor_result3[5]);
  TEST_ASSERT_EQUAL(3, dummy_payload_postprocessor_result3[6]);
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
  RUN_TEST_CASE(api, test_api_handle_interrupt);
  RUN_TEST_CASE(api, test_api_handle_interrupt_max_payload);
}
