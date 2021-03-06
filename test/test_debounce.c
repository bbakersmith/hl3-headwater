#include "debounce.h"
#include "unity.h"
#include "unity_fixture.h"

DebounceButton dummy_button;
DebounceEncoder dummy_encoder;

TEST_GROUP(debounce);

TEST_SETUP(debounce) {
  uint8_t dummy_debounce_threshold = 2;
  dummy_button = debounce_button_new(
    DEBOUNCE_BUTTON_STATE_LOW,
    dummy_debounce_threshold
  );
  dummy_encoder = debounce_encoder_new(
    DEBOUNCE_BUTTON_STATE_LOW,
    dummy_debounce_threshold
  );
};

TEST_TEAR_DOWN(debounce) {};

TEST(debounce, test_debounce_button_update) {
  uint8_t debounce_button_update_tests[13][4] = {
    {0, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_NONE, 0},
    {0, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_NONE, 1},
    {1, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_NONE, 2},
    {1, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_NONE, 3},
    {0, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_NONE, 4},
    {1, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_NONE, 5},
    {1, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_NONE, 6},
    {1, DEBOUNCE_BUTTON_STATE_HIGH, DEBOUNCE_BUTTON_CHANGE_HIGH, 7},
    {1, DEBOUNCE_BUTTON_STATE_HIGH, DEBOUNCE_BUTTON_CHANGE_NONE, 0},
    {0, DEBOUNCE_BUTTON_STATE_HIGH, DEBOUNCE_BUTTON_CHANGE_NONE, 1},
    {0, DEBOUNCE_BUTTON_STATE_HIGH, DEBOUNCE_BUTTON_CHANGE_NONE, 2},
    {0, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_LOW, 3},
    {1, DEBOUNCE_BUTTON_STATE_LOW, DEBOUNCE_BUTTON_CHANGE_NONE, 0}
  };

  uint8_t message[50];
  for(uint8_t i = 0; i < 13; i++) {
    uint8_t dummy_input = debounce_button_update_tests[i][0];
    uint8_t expected_state = debounce_button_update_tests[i][1];
    uint8_t expected_change = debounce_button_update_tests[i][2];
    uint16_t expected_count = debounce_button_update_tests[i][3];

    debounce_button_update(&dummy_button, dummy_input);

    sprintf(message, "Wrong state for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(expected_state, dummy_button.state, message);

    sprintf(message, "Wrong change for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(expected_change, dummy_button.change, message);

    sprintf(message, "Wrong hold_count for iteration %i", i);
    TEST_ASSERT_EQUAL_MESSAGE(expected_count, dummy_button.hold_count, message);
  }
}

TEST(debounce, test_debounce_button_reset_no_change) {
  DebounceButton button = debounce_button_new(DEBOUNCE_BUTTON_STATE_LOW, 1);
  button.state = DEBOUNCE_BUTTON_STATE_LOW;

  debounce_button_reset(&button);

  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_NONE, button.change);

  debounce_button_reset(&button);

  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_NONE, button.change);
}

TEST(debounce, test_debounce_button_reset) {
  DebounceButton button = debounce_button_new(DEBOUNCE_BUTTON_STATE_HIGH, 1);
  button.state = DEBOUNCE_BUTTON_STATE_LOW;

  debounce_button_reset(&button);

  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_HIGH, button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_HIGH, button.change);

  debounce_button_reset(&button);

  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_HIGH, button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_NONE, button.change);
}

TEST(debounce, test_debounce_encoder_update) {
  uint8_t debounce_encoder_update_tests[36][4] = {
    // start right
    {0, 1, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 1, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 1, DEBOUNCE_ENCODER_STATE_RIGHT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    // debounce, ignore
    {1, 1, DEBOUNCE_ENCODER_STATE_RIGHT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 0, DEBOUNCE_ENCODER_STATE_RIGHT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 0, DEBOUNCE_ENCODER_STATE_RIGHT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 0, DEBOUNCE_ENCODER_STATE_RIGHT_2, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 0, DEBOUNCE_ENCODER_STATE_RIGHT_2, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 0, DEBOUNCE_ENCODER_STATE_RIGHT_2, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 0, DEBOUNCE_ENCODER_STATE_RIGHT_3, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 1, DEBOUNCE_ENCODER_STATE_RIGHT_3, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 1, DEBOUNCE_ENCODER_STATE_RIGHT_3, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 1, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_RIGHT},
    {1, 1, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE}, // FIXME 13

    // invalid next step does nothing
    {0, 0, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 0, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 0, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},

    // start left
    {1, 0, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 0, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 0, DEBOUNCE_ENCODER_STATE_LEFT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    // invalid next step, start over
    {0, 1, DEBOUNCE_ENCODER_STATE_LEFT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 1, DEBOUNCE_ENCODER_STATE_LEFT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 1, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 0, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE}, // FIXME 23, setting to RIGHT_1
    {1, 0, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 0, DEBOUNCE_ENCODER_STATE_LEFT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 0, DEBOUNCE_ENCODER_STATE_LEFT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 0, DEBOUNCE_ENCODER_STATE_LEFT_1, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 0, DEBOUNCE_ENCODER_STATE_LEFT_2, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 1, DEBOUNCE_ENCODER_STATE_LEFT_2, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 1, DEBOUNCE_ENCODER_STATE_LEFT_2, DEBOUNCE_ENCODER_CHANGE_NONE},
    {0, 1, DEBOUNCE_ENCODER_STATE_LEFT_3, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 1, DEBOUNCE_ENCODER_STATE_LEFT_3, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 1, DEBOUNCE_ENCODER_STATE_LEFT_3, DEBOUNCE_ENCODER_CHANGE_NONE},
    {1, 1, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_LEFT},
    {1, 0, DEBOUNCE_ENCODER_STATE_NONE, DEBOUNCE_ENCODER_CHANGE_NONE}
  };

  uint8_t dummy_a;
  uint8_t dummy_b;
  uint8_t expected_state;
  uint8_t expected_output;
  uint8_t message[50];
  for(uint8_t i = 0; i < 36; i++) {
    dummy_a = debounce_encoder_update_tests[i][0];
    dummy_b = debounce_encoder_update_tests[i][1];
    expected_state = debounce_encoder_update_tests[i][2];
    expected_output = debounce_encoder_update_tests[i][3];

    debounce_encoder_update(&dummy_encoder, dummy_a, dummy_b);

    sprintf(message, "Wrong state for iteration %i", i);

    TEST_ASSERT_EQUAL_MESSAGE(
      expected_state,
      dummy_encoder.state,
      message
    );

    sprintf(message, "Wrong output for iteration %i", i);

    TEST_ASSERT_EQUAL_MESSAGE(
      expected_output,
      dummy_encoder.output,
      message
    );
  }
}

TEST_GROUP_RUNNER(debounce) {
  RUN_TEST_CASE(debounce, test_debounce_button_update);
  RUN_TEST_CASE(debounce, test_debounce_button_reset_no_change);
  RUN_TEST_CASE(debounce, test_debounce_button_reset);
  RUN_TEST_CASE(debounce, test_debounce_encoder_update);
}
