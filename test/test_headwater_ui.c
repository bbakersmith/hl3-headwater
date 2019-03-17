#include "unity.h"
#include "unity_fixture.h"

#include "headwater_ui.h"

TEST_GROUP(headwater_ui);

TEST_SETUP(headwater_ui) {};

TEST_TEAR_DOWN(headwater_ui) {};

TEST(headwater_ui, test_headwater_ui_modify_with_restrictions) {
  TEST_ASSERT_EQUAL(
    12,
    headwater_ui_modify_with_restrictions(12, 0, 5, 250)
  );

  TEST_ASSERT_EQUAL(
    22,
    headwater_ui_modify_with_restrictions(12, 10, 5, 250)
  );

  TEST_ASSERT_EQUAL(
    5,
    headwater_ui_modify_with_restrictions(12, -2000, 5, 250)
  );

  TEST_ASSERT_EQUAL(
    250,
    headwater_ui_modify_with_restrictions(12, 2000, 5, 250)
  );
}

TEST(headwater_ui, test_headwater_ui_modify_with_restrictions_16) {
  TEST_ASSERT_EQUAL(
    1234,
    headwater_ui_modify_with_restrictions_16(1234, 0, 5, 2500)
  );

  TEST_ASSERT_EQUAL(
    1244,
    headwater_ui_modify_with_restrictions_16(1234, 10, 5, 2500)
  );

  TEST_ASSERT_EQUAL(
    5,
    headwater_ui_modify_with_restrictions_16(1234, -2000, 5, 2500)
  );

  TEST_ASSERT_EQUAL(
    2500,
    headwater_ui_modify_with_restrictions_16(1234, 2000, 5, 2500)
  );
}

TEST(headwater_ui, test_headwater_ui_generic_bpm_update_display_1234) {
  LCD dummy_lcd = lcd_new();

  headwater_ui_generic_bpm_update_display(&dummy_lcd, 4, 1234);

  TEST_ASSERT_EQUAL(LCD__1, dummy_lcd.characters[0]);
  TEST_ASSERT_EQUAL(LCD__2, dummy_lcd.characters[1]);
  TEST_ASSERT_EQUAL(LCD__3, dummy_lcd.characters[2]);
  TEST_ASSERT_EQUAL(LCD__DOT, dummy_lcd.characters[3]);
  TEST_ASSERT_EQUAL(LCD__4, dummy_lcd.characters[4]);
}

TEST(headwater_ui, test_headwater_ui_generic_bpm_update_display_9007) {
  LCD dummy_lcd = lcd_new();

  headwater_ui_generic_bpm_update_display(&dummy_lcd, 4, 9007);

  TEST_ASSERT_EQUAL(LCD__9, dummy_lcd.characters[0]);
  TEST_ASSERT_EQUAL(LCD__0, dummy_lcd.characters[1]);
  TEST_ASSERT_EQUAL(LCD__0, dummy_lcd.characters[2]);
  TEST_ASSERT_EQUAL(LCD__DOT, dummy_lcd.characters[3]);
  TEST_ASSERT_EQUAL(LCD__7, dummy_lcd.characters[4]);
}

TEST(headwater_ui, test_headwater_ui_generic_bpm_update_display_56) {
  LCD dummy_lcd = lcd_new();

  headwater_ui_generic_bpm_update_display(&dummy_lcd, 4, 56);

  TEST_ASSERT_EQUAL(LCD__, dummy_lcd.characters[0]);
  TEST_ASSERT_EQUAL(LCD__, dummy_lcd.characters[1]);
  TEST_ASSERT_EQUAL(LCD__5, dummy_lcd.characters[2]);
  TEST_ASSERT_EQUAL(LCD__DOT, dummy_lcd.characters[3]);
  TEST_ASSERT_EQUAL(LCD__6, dummy_lcd.characters[4]);
}

TEST(headwater_ui, test_headwater_ui_generic_bpm_update_display_0) {
  LCD dummy_lcd = lcd_new();

  headwater_ui_generic_bpm_update_display(&dummy_lcd, 4, 0);

  TEST_ASSERT_EQUAL(LCD__, dummy_lcd.characters[0]);
  TEST_ASSERT_EQUAL(LCD__, dummy_lcd.characters[1]);
  TEST_ASSERT_EQUAL(LCD__0, dummy_lcd.characters[2]);
  TEST_ASSERT_EQUAL(LCD__DOT, dummy_lcd.characters[3]);
  TEST_ASSERT_EQUAL(LCD__0, dummy_lcd.characters[4]);
}

TEST_GROUP_RUNNER(headwater_ui) {
  RUN_TEST_CASE(headwater_ui, test_headwater_ui_modify_with_restrictions);
  RUN_TEST_CASE(headwater_ui, test_headwater_ui_modify_with_restrictions_16);
  RUN_TEST_CASE(
    headwater_ui,
    test_headwater_ui_generic_bpm_update_display_1234
  );
  RUN_TEST_CASE(
    headwater_ui,
    test_headwater_ui_generic_bpm_update_display_9007
  );
  RUN_TEST_CASE(headwater_ui, test_headwater_ui_generic_bpm_update_display_56);
  RUN_TEST_CASE(headwater_ui, test_headwater_ui_generic_bpm_update_display_0);
}
