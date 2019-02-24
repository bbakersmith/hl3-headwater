#include "debounce.h"
#include "unity.h"
#include "unity_fixture.h"

DebounceButton dummy_button;
DebounceRE dummy_rotary_encoder;

TEST_GROUP(debounce);

TEST_SETUP(debounce) {
  uint8_t dummy_debounce_threshold = 2;
  dummy_button = debounce_button_new(
    DEBOUNCE_BUTTON_STATE_LOW,
    dummy_debounce_threshold
  );
  dummy_rotary_encoder = debounce_rotary_encoder_new(
    DEBOUNCE_BUTTON_STATE_LOW,
    dummy_debounce_threshold
  );
};

TEST_TEAR_DOWN(debounce) {};

TEST(debounce, test_debounce_button_update) {
  debounce_button_update(&dummy_button, 0);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 1);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 1);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 0);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 1);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 1);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 1);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_HIGH, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_TRUE, dummy_button.change);

  debounce_button_update(&dummy_button, 1);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_HIGH, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 0);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_HIGH, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 0);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_HIGH, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);

  debounce_button_update(&dummy_button, 0);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_TRUE, dummy_button.change);

  debounce_button_update(&dummy_button, 1);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, dummy_button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, dummy_button.change);
}

TEST(debounce, test_debounce_button_reset_no_change) {
  DebounceButton button = debounce_button_new(DEBOUNCE_BUTTON_STATE_LOW, 1);
  button.state = DEBOUNCE_BUTTON_STATE_LOW;

  debounce_button_reset(&button);

  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, button.change);

  debounce_button_reset(&button);

  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_LOW, button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, button.change);
}

TEST(debounce, test_debounce_button_reset) {
  DebounceButton button = debounce_button_new(DEBOUNCE_BUTTON_STATE_HIGH, 1);
  button.state = DEBOUNCE_BUTTON_STATE_LOW;

  debounce_button_reset(&button);

  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_HIGH, button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_TRUE, button.change);

  debounce_button_reset(&button);

  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_STATE_HIGH, button.state);
  TEST_ASSERT_EQUAL(DEBOUNCE_BUTTON_CHANGE_FALSE, button.change);
}

TEST(debounce, test_debounce_rotary_encoder_update) {
  uint8_t debounce_rotary_encoder_update_tests[36][4] = {
    // start right
    {0, 1, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 1, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 1, DEBOUNCE_RE_STATE_RIGHT_1, DEBOUNCE_RE_OUTPUT_NONE},
    // debounce, ignore
    {1, 1, DEBOUNCE_RE_STATE_RIGHT_1, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 0, DEBOUNCE_RE_STATE_RIGHT_1, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 0, DEBOUNCE_RE_STATE_RIGHT_1, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 0, DEBOUNCE_RE_STATE_RIGHT_2, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 0, DEBOUNCE_RE_STATE_RIGHT_2, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 0, DEBOUNCE_RE_STATE_RIGHT_2, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 0, DEBOUNCE_RE_STATE_RIGHT_3, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 1, DEBOUNCE_RE_STATE_RIGHT_3, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 1, DEBOUNCE_RE_STATE_RIGHT_3, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 1, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_RIGHT},
    {1, 1, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE}, // FIXME 13

    // invalid next step does nothing
    {0, 0, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 0, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 0, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},

    // start left
    {1, 0, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 0, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 0, DEBOUNCE_RE_STATE_LEFT_1, DEBOUNCE_RE_OUTPUT_NONE},
    // invalid next step, start over
    {0, 1, DEBOUNCE_RE_STATE_LEFT_1, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 1, DEBOUNCE_RE_STATE_LEFT_1, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 1, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 0, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE}, // FIXME 23, setting to RIGHT_1
    {1, 0, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 0, DEBOUNCE_RE_STATE_LEFT_1, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 0, DEBOUNCE_RE_STATE_LEFT_1, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 0, DEBOUNCE_RE_STATE_LEFT_1, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 0, DEBOUNCE_RE_STATE_LEFT_2, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 1, DEBOUNCE_RE_STATE_LEFT_2, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 1, DEBOUNCE_RE_STATE_LEFT_2, DEBOUNCE_RE_OUTPUT_NONE},
    {0, 1, DEBOUNCE_RE_STATE_LEFT_3, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 1, DEBOUNCE_RE_STATE_LEFT_3, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 1, DEBOUNCE_RE_STATE_LEFT_3, DEBOUNCE_RE_OUTPUT_NONE},
    {1, 1, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_LEFT},
    {1, 0, DEBOUNCE_RE_STATE_NONE, DEBOUNCE_RE_OUTPUT_NONE}
  };

  uint8_t dummy_a;
  uint8_t dummy_b;
  uint8_t expected_state;
  uint8_t expected_output;
  uint8_t message[50];
  for(uint8_t i = 0; i < 36; i++) {
    dummy_a = debounce_rotary_encoder_update_tests[i][0];
    dummy_b = debounce_rotary_encoder_update_tests[i][1];
    expected_state = debounce_rotary_encoder_update_tests[i][2];
    expected_output = debounce_rotary_encoder_update_tests[i][3];

    debounce_rotary_encoder_update(&dummy_rotary_encoder, dummy_a, dummy_b);

    sprintf(message, "Wrong state for iteration %i", i);

    TEST_ASSERT_EQUAL_MESSAGE(
      expected_state,
      dummy_rotary_encoder.state,
      message
    );

    sprintf(message, "Wrong output for iteration %i", i);

    TEST_ASSERT_EQUAL_MESSAGE(
      expected_output,
      dummy_rotary_encoder.output,
      message
    );
  }
}

TEST_GROUP_RUNNER(debounce) {
  RUN_TEST_CASE(debounce, test_debounce_button_update);
  RUN_TEST_CASE(debounce, test_debounce_button_reset_no_change);
  RUN_TEST_CASE(debounce, test_debounce_button_reset);
  RUN_TEST_CASE(debounce, test_debounce_rotary_encoder_update);
}
