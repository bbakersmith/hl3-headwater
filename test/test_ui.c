#include "ui.h"
#include "unity.h"
#include "unity_fixture.h"

UIField dummy_field_1 = {
  .selected_position = 0x82
};

UIField dummy_field_2 = {
  .selected_position = 0x84
};

UIField dummy_field_3 = {
  .selected_position = 0x86
};

UIScreen dummy_screen;

TEST_GROUP(ui);

TEST_SETUP(ui) {
  UIField dummy_fields[3] = {
    dummy_field_1,
    dummy_field_2,
    dummy_field_3
  };
  uint8_t dummy_fields_count = 3;
  dummy_screen = ui_screen_new(dummy_fields, dummy_fields_count);
};

TEST_TEAR_DOWN(ui) {};

TEST(ui, test_ui_screen_new) {
  TEST_ASSERT_EQUAL(2, dummy_screen.last_index);
  TEST_ASSERT_EQUAL(0, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0, dummy_screen.update_index);
}

TEST(ui, test_ui_selected_position) {
  TEST_ASSERT_EQUAL(0x82, ui_selected_position(&dummy_screen));
}

TEST(ui, test_ui_move_selected) {
  TEST_ASSERT_EQUAL(0, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0x82, ui_selected_position(&dummy_screen));

  uint8_t ui_move_selected_tests[7][3] = {
    {UI_SCREEN_DIRECTION_INC, 1, 0x84},
    {UI_SCREEN_DIRECTION_INC, 2, 0x86},
    {UI_SCREEN_DIRECTION_INC, 0, 0x82},
    {UI_SCREEN_DIRECTION_INC, 1, 0x84},
    {UI_SCREEN_DIRECTION_DEC, 0, 0x82},
    {UI_SCREEN_DIRECTION_DEC, 2, 0x86},
    {UI_SCREEN_DIRECTION_DEC, 1, 0x84}
  };

  uint8_t message[50];
  for(uint8_t i = 0; i < 7; i++) {
    uint8_t dummy_direction = ui_move_selected_tests[i][0];
    uint8_t expected_select_index = ui_move_selected_tests[i][1];
    uint8_t expected_position = ui_move_selected_tests[i][2];

    ui_move_selected(&dummy_screen, dummy_direction);

    sprintf(message, "Wrong select_index for iteration %i", i);

    TEST_ASSERT_EQUAL_MESSAGE(
      expected_select_index,
      dummy_screen.select_index,
      message
    );

    sprintf(message, "Wrong position for iteration %i", i);

    TEST_ASSERT_EQUAL_MESSAGE(
      expected_position,
      ui_selected_position(&dummy_screen),
      message
    );
  }
}

TEST(ui, test_ui_commit_modifier) {
  // TODO test??????
}

TEST_GROUP_RUNNER(ui) {
  RUN_TEST_CASE(ui, test_ui_screen_new);
  RUN_TEST_CASE(ui, test_ui_selected_position);
  RUN_TEST_CASE(ui, test_ui_move_selected);
}
