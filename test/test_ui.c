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

  ui_move_selected(&dummy_screen, UI_SCREEN_DIRECTION_RIGHT);
  TEST_ASSERT_EQUAL(1, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0x84, ui_selected_position(&dummy_screen));

  ui_move_selected(&dummy_screen, UI_SCREEN_DIRECTION_RIGHT);
  TEST_ASSERT_EQUAL(2, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0x86, ui_selected_position(&dummy_screen));

  ui_move_selected(&dummy_screen, UI_SCREEN_DIRECTION_RIGHT);
  TEST_ASSERT_EQUAL(0, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0x82, ui_selected_position(&dummy_screen));

  ui_move_selected(&dummy_screen, UI_SCREEN_DIRECTION_RIGHT);
  TEST_ASSERT_EQUAL(1, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0x84, ui_selected_position(&dummy_screen));

  ui_move_selected(&dummy_screen, UI_SCREEN_DIRECTION_LEFT);
  TEST_ASSERT_EQUAL(0, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0x82, ui_selected_position(&dummy_screen));

  ui_move_selected(&dummy_screen, UI_SCREEN_DIRECTION_LEFT);
  TEST_ASSERT_EQUAL(2, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0x86, ui_selected_position(&dummy_screen));

  ui_move_selected(&dummy_screen, UI_SCREEN_DIRECTION_LEFT);
  TEST_ASSERT_EQUAL(1, dummy_screen.select_index);
  TEST_ASSERT_EQUAL(0x84, ui_selected_position(&dummy_screen));
}

TEST_GROUP_RUNNER(ui) {
  RUN_TEST_CASE(ui, test_ui_screen_new);
  RUN_TEST_CASE(ui, test_ui_selected_position);
  RUN_TEST_CASE(ui, test_ui_move_selected);
}
